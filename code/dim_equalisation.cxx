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
#include <map>      // Dictionary-ish behaviour
#include <unordered_map>

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


struct RetType{
    int counter = 0;
    int naming = 0;
    bool IsTrue;
    unordered_map<string, float> avg;
    unordered_map<string, float> avg_width;
};

int NoOfMaskedPixels(uint16_t dat[10][256*256], int count){
    int pixels = 0;
    int counter = 0;

    
    for (int i=0; i<256*256; i++) {
        for (int j=0; j<count; j++) {
            if (dat[j][i]==0){
                counter++;
            }
        }
        if (counter==count && counter!=0){
            pixels++;
        }
        counter=0;
    }
    return pixels;
}

bool GeneralGTZero(uint16_t dat[10][256*256], int iter, int count){
    int counter = 0;
    for (int i=0; i<count; i++){
        if (dat[i][iter]>0){
            counter +=1;
        }
    }
    if (counter == count){
        return true;
    }
    else{
        return false;
    }
}

bool GeneralETZero(uint16_t dat[10][256*256], int iter, int count){
    int counter=0;
    for (int i=0; i<count; i++){
        if (dat[i][iter]==0){
            counter +=1;
        }
    }
    if (counter == count){
        return true;
    }
    else{
        return false;
    }
}

bool MaskingIsZero(uint16_t dat[10][256], int iter, int count){
    int counter=0;
    for (int i=0; i<count; i++){
        if (dat[i][iter]==0){
            counter +=1;
            if (counter==1)
            {
                return true;
            }
        }
    }
    if (counter<count){
        return true;
    }
    else{
        return false;
    }
}

RetType IsZero(uint16_t dat[10][256*256], int iter, int count, RetType iszeroreturn)
{
    for (int i=0; i<count; i++){
        iszeroreturn.naming = i;
        if (dat[i][iter]>0)
        {
            iszeroreturn.avg["glob_mean" + to_string(i)] += dat[i][iter];
            iszeroreturn.counter +=1;
        }
        else
        {
            iszeroreturn.avg["glob_mean" + to_string(i)] += dat[i][iter];
        }
        }
    if (iszeroreturn.counter == count){
        iszeroreturn.counter = 0;
        iszeroreturn.naming = 0;
        iszeroreturn.IsTrue = true;
    }
    else if (iszeroreturn.counter != count){
        iszeroreturn.counter = 0;
        iszeroreturn.naming = 0;
        iszeroreturn.IsTrue = false;
    }
    return iszeroreturn;
}

RetType IsZeroWidth(uint16_t dat[10][256*256], int iter, int count, RetType iszeroreturn)
{
    for (int i=0; i<count; i++){
        iszeroreturn.naming = i;
        if (dat[i][iter]>0)
        {
            iszeroreturn.avg_width["glob_width" + to_string(i)] = iszeroreturn.avg_width["glob_width" + to_string(i)] + pow(iszeroreturn.avg["glob_mean"+to_string(i)] - dat[i][iter],2);
            iszeroreturn.counter +=1;
        }
        }
    if (iszeroreturn.counter == count){
        iszeroreturn.counter = 0;
        iszeroreturn.naming = 0;
        iszeroreturn.IsTrue = true;
    }
    else if (iszeroreturn.counter != count){
        iszeroreturn.counter = 0;
        iszeroreturn.naming = 0;
        iszeroreturn.IsTrue = false;
    }
    return iszeroreturn;
}


unordered_map<string,int> TrimValues(vector<string> trimlevels, unordered_map<string,int> output, int count)
{
    for (int i=0; i<count; i++){
        if (trimlevels[i]=="0") {output[trimlevels[i]]=0;}
        else if (trimlevels[i]=="1") {output[trimlevels[i]]=1; }
        else if (trimlevels[i]=="2") {output[trimlevels[i]]=2; }
        else if (trimlevels[i]=="3") {output[trimlevels[i]]=3; }
        else if (trimlevels[i]=="4") {output[trimlevels[i]]=4; }
        else if (trimlevels[i]=="5") {output[trimlevels[i]]=5; }
        else if (trimlevels[i]=="6") {output[trimlevels[i]]=6; }
        else if (trimlevels[i]=="7") {output[trimlevels[i]]=7; }
        else if (trimlevels[i]=="8") {output[trimlevels[i]]=8; }
        else if (trimlevels[i]=="9") {output[trimlevels[i]]=9; }
        else if (trimlevels[i]=="A" || trimlevels[i]=="a") {output[trimlevels[i]]=10;}
        else if (trimlevels[i]=="B" || trimlevels[i]=="b") {output[trimlevels[i]]=11;}
        else if (trimlevels[i]=="C" || trimlevels[i]=="c") {output[trimlevels[i]]=12;}
        else if (trimlevels[i]=="D" || trimlevels[i]=="d") {output[trimlevels[i]]=13;}
        else if (trimlevels[i]=="E" || trimlevels[i]=="e") {output[trimlevels[i]]=14;}
        else if (trimlevels[i]=="F" || trimlevels[i]=="f") {output[trimlevels[i]]=15;}
    }
    return output;
    
}


float Scale (int trim1, int trim2){
    float ts = fabs(pow(0.02758*(trim1-trim2),3) - 0.6792*pow((trim1-trim2),2) +19.48*(trim1-trim2) +1996);
    return ts;
}

//int CalcTarget(uint16_t[10][256*256], unordered_map<string, int> trimlevels)
//{
//
//
//}






bool Masking(uint16_t dat[10][256*256], int iter, int count, unordered_map<string, int> diff, unordered_map<string, int> trims){
    int counter=0;
    int counter_diff=0;
    int counter_trims=0;
    for (int i=0; i<count; i++){
        if (dat[i][iter]==0){
            counter +=1;
            if (counter==1)
            {
                return true;
            }
        }
    }
    if (counter == count){
        return true;
    }
    else{
        return false;
    }
}




//  unordered_map<string, float> Trim_scales;
//  unordered_map<string, int> Trims;
//  uint16_t predicting [10][256*256];
//  unordered_map<string, int> difference;
//  unordered_map<string, int> achieved_means;
//  unordered_map<string, float> achieved_widths;

//if (mean_trim0[i]==0 || mean_trimF[i]==0 || trim>15 || trim<0 || diff>dacRange || mean_trim5[i]==0 || mean_trimA[i]==0 || diff_0_5>dacRange || diff_F_5>dacRange) {
//  if (mean_trim0[i]==0 && mean_trimF[i]==0 && mean_trim5[i]==0 && mean_trimA[i] ==0) mask = 1;
//  else if (mean_trim0[i]==0) mask = 2;
//  else if (mean_trimF[i]==0) mask = 3;
//  else if (trim>15 || trim<0 || trim_0_5>15 || trim_0_5<0 || trim_F_5>15 || trim_F_5<0) mask = 4;
//  else if (diff>dacRange) mask = 5;
//  else if (diff_0_5>dacRange) mask = 5;
//  else if (diff_F_5>dacRange) mask = 5;
//  else if (mean_trim5[i]==0) mask = 6;
//  else if (mean_trimA[i]==0) mask = 7;
//  else mask = 8; // Should not happen




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
    unordered_map<string,int> inputlevels;
    inputlevels = TrimValues(trimvec, inputlevels, argc-2);
    
    int dacRange = 25; // Tuneable parameter
    uint16_t matrixarray[10][256*256];
    for (int i=0; i<argc-2; i++)
    {
        load_mean(prefix+"_Trim"+argv[i+2]+"_Noise_Mean.csv", matrixarray[i]);
    }
    
    
    int mask_pixels_init = 0;
    mask_pixels_init = NoOfMaskedPixels(matrixarray, argc-2);
    cout << "number of masked pixels initially: " << mask_pixels_init << endl;
    
    
    // === Load Trim 0 and Trim F Means ===
  uint16_t mean_trim0[256*256];
  load_mean(prefix + "_Trim0_Noise_Mean.csv", mean_trim0);
  uint16_t mean_trimF[256*256];
  load_mean(prefix + "_TrimF_Noise_Mean.csv", mean_trimF);
  uint16_t mean_trim5[256*256];
  load_mean(prefix + "_Trim5_Noise_Mean.csv", mean_trim5);
  uint16_t mean_trimA[256*256];
  load_mean(prefix + "_TrimA_Noise_Mean.csv", mean_trimA);
    


    
  // === Calculate Target ===
  cout << "[dim_equalisation] Equalising" << endl;
  int glob_mean_trim0 = 0;
  int glob_mean_trimF = 0;
  int glob_mean_trim5 = 0;
  int glob_mean_trimA = 0;
    // === added for test ===/
    unordered_map<string, float> means;
    int test_mean_trim0 = 0;
    int test_mean_trimF = 0;
    int test_mean_trim5 = 0;
    int test_mean_trimA = 0;
    
    
    int nohits = 0;
    RetType iszeroreturn;
    for (int j=0; j<256*256; j++){
        iszeroreturn = IsZero(matrixarray, j, argc-2, iszeroreturn);
        if (iszeroreturn.IsTrue){
            means = iszeroreturn.avg;
            nohits++;
        }
    }

    
    for (int i =0; i<argc-2;i++)
    {
        means["glob_mean" + to_string(i)] /= nohits;
        iszeroreturn.avg["glob_mean" + to_string(i)] /=nohits;
//        line below works, but overwrites second argument?
//        means["glob_mean" + trimvec[i]] = means["glob_mean" + to_string(i)];
    }
    int targettest=0;
    for (int i=0; i<argc-2; i++){
        targettest += means["glob_mean" + to_string(i)];
    }
    targettest /= (argc-2);
    cout << "test target: " << targettest << endl;

    
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



    
  if (nhits==0) {
    cout << "[dim_equalisation] FAILED: Threshold scan has empty output file" << endl;
    return 0;
  }
  glob_mean_trim0 /= nhits;
  glob_mean_trimF /= nhits;
  glob_mean_trim5 /= nhits;
  glob_mean_trimA /= nhits;
  int target = (glob_mean_trim0 + glob_mean_trimF + glob_mean_trim5 + glob_mean_trimA)/4;
  
    unordered_map<string, float> widths;

    
  float glob_width_trim0 = 0;
  float glob_width_trimF = 0;
  float glob_width_trim5 = 0;
  float glob_width_trimA = 0;
//    RetType returnwidth;
  for (int j=0; j<256*256; ++j) {
      iszeroreturn = IsZeroWidth(matrixarray, j, argc-2, iszeroreturn);
    if (iszeroreturn.IsTrue) {
        widths = iszeroreturn.avg_width;

      glob_width_trim0 += pow(glob_mean_trim0 - mean_trim0[j], 2);
      glob_width_trimF += pow(glob_mean_trimF - mean_trimF[j], 2);
      glob_width_trim5 += pow(glob_mean_trim5 - mean_trim5[j], 2);
      glob_width_trimA += pow(glob_mean_trimA - mean_trimA[j], 2);
    }
  }
    
    
    for (int i=0; i <argc-2; i++){
        iszeroreturn.avg_width["glob_width"+to_string(i)] = sqrt(iszeroreturn.avg_width["glob_width"+to_string(i)] / (nohits-1));
    }
    
//    for (int i=0; i<argc-2; i++){
//        cout << iszeroreturn.avg_width["glob_width"+to_string(i)] << endl;
//    }
    
  glob_width_trim0 = sqrt(glob_width_trim0/(nhits-1));
  glob_width_trimF = sqrt(glob_width_trimF/(nhits-1));
  glob_width_trim5 = sqrt(glob_width_trim5/(nhits-1));
  glob_width_trimA = sqrt((glob_width_trimA)/(nhits-1));

  
        
        
    
  // === Finished up until here ===
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
    
        
        
        
        
  
    unordered_map<string, float> Trim_scales;
    unordered_map<string, int> Trims;
//    uint16_t predicting [10][256*256];
    unordered_map<string, int> difference;
    unordered_map<string, int> achieved_means;
    unordered_map<string, float> achieved_widths;
          
    
////     === Test to get prediction running on all inputs ===
//    for (int i=0; i<argc-2; i++){
//
//        Trim_scales["trim"] =
//
//        Trims[trimvec[i]] = round((targettest - matrixarray[i][j])/Trim_scales[])
//        trim_F_5 = round((target - mean_trim5[i])/trim_scale_F_5);
//    }
//
//
//    for (int j=0; j<256*256; j++){
//
//
//    }
    
    
    
    
    
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
    float trscaletry;
    

  for (int i=0; i<256*256; ++i) {
// === replace with 3rd degree polynomial ===

    trim_scale = 1.*(mean_trimF[i] - mean_trim0[i])/16;
    trim = round((target - mean_trim0[i])/trim_scale);
            
      // === Added april 21th ===
    trim_scale_F_5 = 1.*(mean_trimF[i] - mean_trim5[i])/11;
    trim_F_5 = round((target - mean_trim5[i])/trim_scale_F_5);

      // === Added april 22nd ===
    trim_scale_0_5 = 1.*(mean_trim0[i] - mean_trim5[i])/6;
    trim_0_5 = round((target - mean_trim0[i])/trim_scale_0_5);

      
    mask = 0;
    predict[i] = mean_trim0[i] + round(trim*trim_scale);
    
    predict_F_5[i] = mean_trim5[i] + round(trim_F_5*trim_scale_F_5);
    predict_0_5[i] = mean_trim0[i] + round(trim_0_5*trim_scale_0_5);

    diff = fabs(predict[i] - target);
    diff_F_5 = fabs(predict_F_5[i] - target);
    diff_0_5 = fabs(predict_0_5[i] - target);
      
      
    if (mean_trim0[i]==0 || mean_trimF[i]==0 || trim>15 || trim<0 || diff>dacRange || mean_trim5[i]==0 || mean_trimA[i]==0 || diff_0_5>dacRange || diff_F_5>dacRange) {
      if (mean_trim0[i]==0 && mean_trimF[i]==0 && mean_trim5[i]==0 && mean_trimA[i] ==0) mask = 1;
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
    
    for (int i=0; i<argc-2; i++ ){
        cout << "Trim" + trimvec[i] << ":   " << round(means["glob_mean" + to_string(i)]) << endl;
    }
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
