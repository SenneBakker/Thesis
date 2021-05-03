// ======================================
//
// Libraries
//
// ======================================
#include <iostream> // Needed to use std namespace + user input
#include <math.h>   // Needed to use pow and sqrt
#include <stdint.h> // Needed to use uint8_t
#include <stdio.h>  // Needed to use printf
#include <string>
#include <sstream>  // string splitting
#include <vector>   // string splitting
#include <list>
#include <glob.h>

using namespace std;

// ======================================
//
// Global variables
//
// ======================================

int mean_trim0[256*256];


// ======================================
//
// Helper Functions
//
// ======================================

/* Goal: Generic function to split strings into substrings
 */
vector<string> split(const string& s, char delimiter)
{
   vector<string> tokens;
   string token;
   istringstream tokenStream(s);
   while (getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}



// ======================================
//
// Primary Functions
//
// ======================================
/* Goal: Load the pixel noise mean from file
 * Input: (1) data file, (2) matrix for means
 */
int load_mean(string filename, uint16_t* matrix)
{
  // Initialise
  FILE *next_file = fopen(filename.c_str(), "r");
  char next_line[2500];
  vector<string> next_vals;
  int id = 0;

  // Clear
  for (int i=0; i<256*256; ++i) matrix[i] = 0;

  for (int l=0; l<256; ++l) {
    fgets(next_line, 2500, next_file);
    next_vals = split(next_line, ',');
    for (int val=0; val<256; ++val) {
      matrix[id] += stoi(next_vals[val]);
      id++;
    }
  }
  fclose(next_file);

  return 0;
}



// ======================================
//
// Main
//
// ======================================
int main(int argc, char* argv[])
{
  // === Input ===
  if (argc<4) {
    cout << "[dim_equalisation] FAILED: Input was incorrect" << endl;
    cout << "[dim_equalisation] Usage: ./dim_equalisation prefix Trim1 trim2 ... " << endl;
    cout << "\n\n";
    return 0;
  }
  string prefix = argv[1];
    
    // === initializing vector to track trim levels === //
    vector<string> trimvec(10);
    for (int i=2; i<argc; i++){
        trimvec[i-2] = argv[i];
    }
    
    int dacRange = 25; // Tuneable parameter
    
    
// === should be array of matrices of dim. (256x256). Should be possible ===/
    uint16_t matrixarray[10][256*256];
//    cout << endl << prefix+"_Trim"+argv[4]+"_Noise_Mean.csv"<< endl;
//    load_mean(prefix+"_Trim"+argv[4]+"_Noise_Mean.csv", matrixarray[0]);
//    load_mean(prefix+"_Trim"+argv[3]+"_Noise_Mean.csv", matrixarray[1]);
    
    for (int i=0; i<argc-2; i++)
    {
//        cout << argv[i+2] << endl << i << endl;
        load_mean(prefix+"_Trim"+argv[i+2]+"_Noise_Mean.csv", matrixarray[i]);
//        cout << *matrixarray[i] << endl;
    }

  // === Load Trim 0 and Trim F Means ===
  uint16_t mean_trim0[256*256];
  load_mean(prefix + "_Trim0_Noise_Mean.csv", mean_trim0);
  uint16_t mean_trimF[256*256];
  load_mean(prefix + "_TrimF_Noise_Mean.csv", mean_trimF);
  uint16_t mean_trim5[256*256];
  load_mean(prefix + "_Trim5_Noise_Mean.csv", mean_trim5);
  uint16_t mean_trimA[256*256];
  load_mean(prefix + "_TrimA_Noise_Mean.csv", mean_trimA);
    
//    for (int i=0; i<10; i++){
//        cout << matrixarray[1][i] << endl;
//    }

    
  // === Calculate Target ===
  cout << "[dim_equalisation] Equalising" << endl;
  int glob_mean_trim0 = 0;
  int glob_mean_trimF = 0;
  int glob_mean_trim5 = 0;
  int glob_mean_trimA = 0;
    // === added for test ===/
    int test_mean_trim0 = 0;
    int test_mean_trimF = 0;
    int test_mean_trim5 = 0;
    int test_mean_trimA = 0;
    
    
  // === iterating over array of matrices seems to be working.
  int nhits = 0;
  for (int i=0; i<256*256; ++i) {
    if (mean_trim0[i]>0 && mean_trimF[i]>0 && mean_trim5[i]>0 && mean_trimA[i]>0) {
      glob_mean_trim0 += mean_trim0[i];
      glob_mean_trimF += mean_trimF[i];
      glob_mean_trim5 += mean_trim5[i];
      glob_mean_trimA += mean_trimA[i];
        test_mean_trim0 += matrixarray[0][i];
        test_mean_trim5 += matrixarray[1][i];
        test_mean_trimA += matrixarray[2][i];
        test_mean_trimF += matrixarray[3][i];
      nhits++;
    }
  }
    
    cout << "mean tests:" << endl;
    cout << test_mean_trim0 << endl << test_mean_trim5 << endl << test_mean_trimA << endl << test_mean_trimF << endl;
    cout << "\n\n";
    cout << "mean normal:" << endl;
    cout<< glob_mean_trim0 << endl << glob_mean_trim5 << endl << glob_mean_trimA << endl << glob_mean_trimF << endl;
    
    
    
    
    
  if (nhits==0) {
    cout << "[dim_equalisation] FAILED: Threshold scan has empty output file" << endl;
    return 0;
  }
  glob_mean_trim0 /= nhits;
  glob_mean_trimF /= nhits;
  glob_mean_trim5 /= nhits;
  glob_mean_trimA /= nhits;
  int target = (glob_mean_trim0 + glob_mean_trimF + glob_mean_trim5 + glob_mean_trimA)/4;
  
  float glob_width_trim0 = 0;
  float glob_width_trimF = 0;
  float glob_width_trim5 = 0;
  float glob_width_trimA = 0;
  for (int i=0; i<256*256; ++i) {
    if (mean_trim0[i]>0 && mean_trimF[i]>0 && mean_trim5[i]>0 & mean_trimA[i]>0) {
      glob_width_trim0 += pow(glob_mean_trim0 - mean_trim0[i], 2);
      glob_width_trimF += pow(glob_mean_trimF - mean_trimF[i], 2);
      glob_width_trim5 += pow(glob_mean_trim5 - mean_trim5[i], 2);
      glob_width_trimA += pow(glob_mean_trimA - mean_trimA[i], 2);
    }
  }
  glob_width_trim0 = sqrt(glob_width_trim0/(nhits-1));
  glob_width_trimF = sqrt(glob_width_trimF/(nhits-1));
  glob_width_trim5 = sqrt(glob_width_trim5/(nhits-1));
  glob_width_trimA = sqrt((glob_width_trimA)/(nhits-1));

  
  // === Calculate optimal trim ===
  string name_mask = prefix + "_Matrix_Mask.csv";
  FILE *file_mask = fopen(name_mask.c_str(), "w");
  string name_trim = prefix + "_Matrix_Trim.csv";
  FILE *file_trim = fopen(name_trim.c_str(), "w");
  string name_pred = prefix + "_TrimBest_Noise_Predict.csv";
  FILE *file_pred = fopen(name_pred.c_str(), "w");
  // === Added april 15th ===
  string name_lpred = prefix + "_TrimBest_Noise_Predict_F_5.csv";
  FILE *file_lpred = fopen(name_lpred.c_str(), "w");
  // === Added april 22nd ===
  string name_0pred = prefix + "_TrimBest_Noise_Predict_0_5.csv";
  FILE *file_0pred = fopen(name_0pred.c_str(), "w");
    
  
  float trim_scale;
  float trim_scale_F_5;
  float trim_scale_0_5;
  int trim;
  int trim_F_5;
  int trim_0_5;
  int mask;
  int predict[256*256];
  int predict_F_5[256*256];
  int predict_0_5[256*256];
  int diff;
  int diff_F_5;
  int diff_0_5;
  long nmasked = 0;
  int achieved_mean = 0;
  int achieved_mean_F_5 = 0;
  int achieved_mean_0_5 = 0;
  float achieved_width = 0;
  float achieved_width_F_5 = 0;
  float achieved_width_0_5 = 0;
  
  for (int i=0; i<256*256; ++i) {
    trim_scale = 1.*(mean_trimF[i] - mean_trim0[i])/16;
    trim = round((target - mean_trim0[i])/trim_scale);
    // === Added april 21th ===
    trim_scale_F_5 = 1.*(mean_trimF[i] - mean_trim5[i])/16;
    trim_F_5 = round((target - mean_trim5[i])/trim_scale_F_5);
    // === Added april 22nd ===
    trim_scale_0_5 = 1.*(mean_trim0[i] - mean_trim5[i])/16;
    trim_0_5 = round((target - mean_trim0[i])/trim_scale_0_5);

      
    mask = 0;
    predict[i] = mean_trim0[i] + round(trim*trim_scale);
    
    predict_F_5[i] = mean_trim5[i] + round(trim_F_5*trim_scale_F_5);
    predict_0_5[i] = mean_trim0[i] + round(trim_0_5*trim_scale_0_5);

    diff = fabs(predict[i] - target);
    diff_F_5 = fabs(predict_F_5[i] - target);
    diff_0_5 = fabs(predict_0_5[i] - target);
      
    // === Should be different since trim 5 is added. ===
    if (mean_trim0[i]==0 || mean_trimF[i]==0 || trim>15 || trim<0 || diff>dacRange || mean_trim5[i]==0 || mean_trimA[i]==0 || diff_0_5>dacRange || diff_F_5>dacRange) {
      if (mean_trim0[i]==0 && mean_trimF[i]==0 && mean_trim5[i]==0) mask = 1;
      else if (mean_trim0[i]==0) mask = 2;
      else if (mean_trimF[i]==0) mask = 3;
      else if (trim>15 || trim<0 || trim_0_5>15 || trim_0_5<0 || trim_F_5>15 || trim_F_5<0) mask = 4;
      else if (diff>dacRange) mask = 5;
      else if (diff_0_5>dacRange) mask = 5;
      else if (diff_F_5>dacRange) mask = 5;
      else if (mean_trim5[i]==0) mask = 6;
      else if (mean_trimA[i]==0) mask = 7;
      else mask = 8; // Should not happen

      trim = 0;
      predict[i] = 0;
      nmasked++;
    } else {
      achieved_mean += predict[i];
      achieved_mean_F_5 += predict_F_5[i];
      achieved_width_0_5 += predict_0_5[i];
    }
    // Save results
    if (i%256==255) {
      fprintf(file_mask, "%d\n", mask);
      fprintf(file_trim, "%d\n", trim);
      fprintf(file_pred, "%04d\n", predict[i]);
      fprintf(file_lpred, "%04d\n", predict_F_5[i]);
      fprintf(file_0pred, "%04d\n", predict_0_5[i]);
    } else {
      fprintf(file_mask, "%d,", mask);
      fprintf(file_trim, "%d,", trim);
      fprintf(file_pred, "%04d, ", predict[i]);
      fprintf(file_lpred, "%04d, ", predict_F_5[i]);
      fprintf(file_0pred, "%04d, ", predict_0_5[i]);
    }
  }
  fclose(file_mask);
  fclose(file_trim);
  fclose(file_lpred);
  fclose(file_pred);
  fclose(file_0pred);
  achieved_mean /= (256*256-nmasked);
  achieved_mean_0_5 /= (256*256-nmasked);
  achieved_mean_F_5 /= (256*256-nmasked);
  
  for (int i=0; i<256*256; ++i) {
    if (predict[i]>0) achieved_width += pow(predict[i] - achieved_mean, 2);
      if (predict_0_5[i]>0) achieved_width_0_5 += pow(predict_0_5[i] - achieved_width_0_5,2);
      if (predict_F_5[i]>0) achieved_mean_F_5 += pow(predict_F_5[i] - achieved_mean_F_5,2);
  }
  achieved_width = sqrt(achieved_width/(256*256-nmasked-1));
  achieved_width_0_5 = sqrt(achieved_width_0_5/(256*256-nmasked-1));
  achieved_width_F_5 = sqrt(achieved_width_F_5/(256*256-nmasked-1));
    
    
  cout << "[dim_equalisation] Summary" << endl;
  cout << "  Trim 0 distribution: " << glob_mean_trim0 << " +/- " << round(glob_width_trim0) << endl;
  cout << "  Trim 5 distribution: " << glob_mean_trim5 << " +/- " << round(glob_width_trim5) << endl;
  cout << "  Trim A distribution: " << glob_mean_trimA << " +/- " << round(glob_width_trimA) << endl;
  cout << "  Trim F distribution: " << glob_mean_trimF << " +/- " << round(glob_width_trimF) << endl;
    cout << "\n\n";
    
  cout << "  Mean of widths: " << (glob_width_trim0 + glob_width_trim5 + glob_width_trimA + glob_width_trimF)/4 << endl;

  cout << "  Equalisation Target: " << target << endl;
  char buffer[25];
  sprintf(buffer, "  Achieved: %d +/- %.1f", achieved_mean, achieved_width);
  cout << buffer << endl;
  cout << "  Masked Pixels: " << nmasked << endl;
  
  // === Test Pulse Pattern ===
  // for completeness
  string name_tp = prefix + "_Matrix_TP.csv";
  FILE *file_tp = fopen(name_tp.c_str(), "w");
  for (int i=0; i<256*256; ++i) {
    if (i==256*255)fprintf(file_tp, "1,");
    else if (i%256==255) fprintf(file_tp, "0\n");
    else fprintf(file_tp, "0,");
  }
  //for (int i=0; i<255; ++i) fprintf(file_tp, "1,");
  //fprintf(file_tp, "1\n");
  fclose(file_tp);
  
  return 0;
}
