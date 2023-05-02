/// @file

#include <iostream>
#include <fstream>
#include <string.h>
#include <errno.h>

#include "wavfile_mono.h"
#include "pitch_analyzer.h"

#include "docopt.h"

#define FRAME_LEN   0.030 /* 30 ms. */
#define FRAME_SHIFT 0.015 /* 15 ms. */

using namespace std;
using namespace upc;

static const char USAGE[] = R"(
get_pitch - Pitch Estimator 

Usage:
    get_pitch [options] <input-wav> <output-txt>
    get_pitch (-h | --help)
    get_pitch --version

Options:
    -h, --help  Show this screen
    --version   Show the version of the project
    -w <window-type>, --window=<window-type>  Define windowing to use [default: RECT]
    -p1 FLOAT, --param1=REAL  Define parameter 1  [default: 0.7]
    -p2 REAL, --param2=REAL  Define parameter 2  [default: 0.7]
    -p3 REAL , --param3=REAL  Define parameter 3  [default: -40.0]

Arguments:
    input-wav   Wave file with the audio signal
    output-txt  Output file: ASCII file with the result of the estimation:
                    - One line per frame with the estimated f0
                    - If considered unvoiced, f0 must be set to f0 = 0
)";

int main(int argc, const char *argv[]) {
  
	/// \TODO 
	///  Modify the program syntax and the call to **docopt()** in order to
	///  add options and arguments to the program.
    std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
        {argv + 1, argv + argc},	// array of arguments, without the program name
        true,    // show help if requested
        "2.0");  // version string

	std::string input_wav = args["<input-wav>"].asString();
	std::string output_txt = args["<output-txt>"].asString();
  std::string wintype = args["--window"].asString();


  float param1 = stof(args["--param1"].asString());
  float param2 = stof(args["--param2"].asString());
  float param3 = stof(args["--param3"].asString());

  // cout << "param1: " << param1 <<" param2: " << param2 << " param3: " <<param3 << " ";

  PitchAnalyzer::Window windowt;
   
  
  // Read input sound file
  unsigned int rate;
  vector<float> x;
  if (readwav_mono(input_wav, rate, x) != 0) {
    cerr << "Error reading input file " << input_wav << " (" << strerror(errno) << ")\n";
    return -2;
  }

  int n_len = rate * FRAME_LEN;
  int n_shift = rate * FRAME_SHIFT;


  // Define analyzer
  
  if(wintype=="HAMMING") {
    windowt=PitchAnalyzer::HAMMING;
  }
  else {
    windowt=PitchAnalyzer::RECT;
  }

  PitchAnalyzer analyzer(n_len, rate, windowt, 50, 500, param1, param2, param3);

  /// \TODO
  /// Preprocess the input signal in order to ease pitch estimation. For instance,
  /// central-clipping or low pass filtering may be used.
  
  // Iterate for each frame and save values in f0 vector
  int cont = 0;
  vector<float>::iterator iX;
  vector<float> f0;
  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift) {
    float f = analyzer(iX, iX + n_len, cont);
    cont++;
    f0.push_back(f);
  }

  /// \TODO
  /// Postprocess the estimation in order to supress errors. For instance, a median filter
  /// or time-warping may be used.
  //for(int k = 1; k<f0.size()-1; k++){
  //  float midF;
  //  if((f0[k]<f0[k-1] && f0[k-1]<f0[k+1]) || (f0[k]>f0[k-1] && f0[k-1]>f0[k+1])) midF=f0[k-1];
  //  else if ((f0[k]<f0[k+1] && f0[k+1]<f0[k-1]) || (f0[k]>f0[k+1] &&f0[k+1]>f0[k-1])) midF=f0[k+1];
  //  else midF = f0[k];
  //  f0[k]=midF;
  //}

  for(unsigned int l = 1; l < f0.size(); l++){
    if(f0[l-1] != 0 && f0[l]!=0){
      if(f0[l]>f0[l-1]*1.8){
        f0[l] = f0[l-1];
      }
    }
    
  }

  // Write f0 contour into the output file
  ofstream os(output_txt);
  if (!os.good()) {
    cerr << "Error reading output file " << output_txt << " (" << strerror(errno) << ")\n";
    return -3;
  }

  os << 0 << '\n'; //pitch at t=0
  for (iX = f0.begin(); iX != f0.end(); ++iX) 
    os << *iX << '\n';
  os << 0 << '\n';//pitch at t=Dur

  return 0;
}
