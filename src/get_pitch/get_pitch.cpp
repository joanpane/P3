/// @file

#include <iostream>
#include <fstream>
#include <string.h>
#include <errno.h>
#include <math.h>

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
    -1 FLOAT, --param1=FLOAT  Define parameter 1 (r[max]/r[0]) for voice detection [default: 0.406]
    -2 FLOAT, --param2=FLOAT  Define parameter 2 (r[1]/r[0]) [default: 0.5]
    -3 FLOAT, --param3=FLOAT  Define parameter 3 (pot) [default: -46]
    -c FLOAT, --centerclipth=FLOAT  Define center clipping threshhold [default: 0.0001]

Arguments:
    input-wav   Wave file with the audio signal
    output-txt  Output file: ASCII file with the result of the estimation:
                    - One line per frame with the estimated f0
                    - If considered unvoiced, f0 must be set to f0 = 0
)";

vector<float> get_data(vector<float> x){
  vector<float> data(3);
  vector<float> r(x.size()/2);
  for (unsigned int l = 0; l < r.size(); ++l) {
      r[l] = 0;

      for(unsigned int n=l; n<x.size(); n++){ // i n=0?
        r[l] += x[n]*x[n-l];
      }
      r[l] /= x.size();
  }
  if (r[0] == 0.0F) //to avoid log() and divide zero 
    r[0] = 1e-10; 
  
  int iRMax = 0;
  float Rmax = 0;

  for(long unsigned int i=1; i<r.size(); i++){
    if(r[i]>Rmax){ 
      Rmax = r[i];
      iRMax = i;
    }
  }

  data[0]= 10 * log10(r[0]);;
  data[1]=r[1]/r[0];
  data[2]=r[iRMax]/r[0];

  return data; 
}

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
  float ccth = stof(args["--centerclipth"].asString());

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

  int F0_min = 50;
  int F0_max = 500;

  PitchAnalyzer analyzer(n_len, rate, windowt, F0_min, F0_max, param1, param2, param3);

  /// \TODO
  /// Preprocess the input signal in order to ease pitch estimation. For instance,
  /// central-clipping or low pass filtering may be used.

  for(unsigned int k=0; k<x.size(); k++){
    if(abs(x[k])<ccth){
      x[k]=0;
    }
  }
  
  // Iterate for each frame and save values in f0 vector
  int cont = 0;
  vector<float>::iterator iX;
  vector<float> f0;
  vector<float> pot;
  vector<float> r1norm;
  vector<float> rmaxnorm;

  ofstream os2("prueba.rmaxnorm");
  if (!os2.good()) {
    cerr << "Error reading output file " << "data_out.txt" << " (" << strerror(errno) << ")\n";
    return -3;
  }

  for (iX = x.begin(); iX + n_len < x.end(); iX = iX + n_shift) {
    float f = analyzer(iX, iX + n_len, cont);
    if (f) 
      cont++;
    f0.push_back(f);

    //For data:
    std::vector<float> x2(n_len); //local copy of input frame, size N
    std::copy(iX, iX + n_len, x2.begin()); //copy input values into local vector x
    vector<float> data = get_data(x2);
    os2 << data[2] << '\n';
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

  vector<float> element;     
  vector<float> f0_filtrado;          
  f0_filtrado.push_back(f0[0]);       
  for (unsigned int l=1; l<f0.size()-1; l++){   
      for(int r=-1; r<2; r++){    
        element.push_back(f0[l+r]);   
      }
      sort(element.begin(),element.end());   
      f0_filtrado.push_back(element[1]);    
      element.clear();    
    }
  f0_filtrado.push_back(f0[f0.size()-1]); 
  f0 = f0_filtrado;



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
