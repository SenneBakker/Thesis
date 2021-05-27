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
struct MaskingType{
    int mask=0;
    int masked=0;
    bool NotMasked=false;
    
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

MaskingType MaskingIsZero(uint16_t dat[10][256*256], int iter, int trim, int diff, int dac, MaskingType ret){
    ret.mask = 0;
    ret.NotMasked=false;
    if (dat[0][iter]==0 || dat[1][iter]==0 || diff>dac || trim>15 || trim<0){
        ret.masked++;
    }
    if (dat[0][iter]==0){
        ret.mask = 1;
    }
    if (dat[1][iter]==0){
        ret.mask = 2;
    }
    if (diff>dac){
        ret.mask=3;
    }
    if (trim>15 || trim<0){
        ret.mask = 4;
    }
    else {
        ret.NotMasked = true;
    }
    return ret;
}

RetType IsZero(uint16_t dat[10][256*256], int iter, int count, RetType iszeroreturn)
{
    for (int i=0; i<count; i++){
        iszeroreturn.naming = i;
        if (dat[i][iter]>0)
        {
            iszeroreturn.counter +=1;
        }
        if (iszeroreturn.counter == count){
            for (int j=0; j<count; j++){
            iszeroreturn.avg["glob_mean"+to_string(j)] += dat[j][iter];
            }
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
            iszeroreturn.counter +=1;
        }
        if (iszeroreturn.counter == count){
            for (int j=0; j<count; j++){
                iszeroreturn.avg_width["glob_width" + to_string(j)] = iszeroreturn.avg_width["glob_width" + to_string(j)] + pow(iszeroreturn.avg["glob_mean"+to_string(j)] - dat[j][iter],2);
            }
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

unordered_map<string,float> TrimValues(vector<string> trimlevels, unordered_map<string,float> output, int count)
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

float Scale2Trims(unordered_map<string,float> levels, vector<string> trimvec, int iter, uint16_t dat[10][256*256], int count){
    float trimscale=0;
    for (int i=0; i<count; i++){
        trimscale += pow(-1,i)*dat[i][iter];
    }
    trimscale = fabs(trimscale)/ (fabs(levels[trimvec[0]]-levels[trimvec[1]])+1);
    return trimscale;
}

//int CalculateTarget(unordered_map<string, int> means, vector<string> trimvec, unordered_map<string, float> levels){
//    int target = 0;
//
//
//
//}



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
    unordered_map<string,float> inputlevels;
    inputlevels = TrimValues(trimvec, inputlevels, argc-2);
    
    int dacRange = 25; // Tuneable parameter
    
    uint16_t matrixarray[10][256*256];
    // === load means ===
    for (int i=0; i<argc-2; i++)
    {
        load_mean(prefix+"_Trim"+argv[i+2]+"_Noise_Mean.csv", matrixarray[i]);
    }
    
//    === Hard coding the 0 trim for target ===
    load_mean(prefix+"_Trim0_Noise_Mean.csv", matrixarray[2]);
    
    int mask_pixels_init = 0;
    mask_pixels_init = NoOfMaskedPixels(matrixarray, argc-2);
    cout << "  Number of masked pixels initially: " << mask_pixels_init << endl;

    // === Calculate Target ===
    cout << "  [dim_equalisation] Equalising" << endl;
    
    unordered_map<string, float> means;
    // === calculate averages
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
    }

    for (int i =0; i<256*256; i++){
        means["glob_mean2"]+=matrixarray[2][i];
    }
    means["glob_mean2"]/=nohits;
    
    
    
//    === dynamic target ====
//    int target=0;
//    for (int i=0; i<argc-2; i++){
//        target += means["glob_mean" + to_string(i)];
//    }
//    target /= (argc-2);
//    cout << argv[3] << endl;
    
    
////    === hard coded target ===
    int target = 0;
    target = (means["glob_mean2"] + means["glob_mean"+to_string(1)])/2;
    
    

    if (nohits==0) {
        cout << "[dim_equalisation] FAILED: Threshold scan has empty output file" << endl;
        return 0;
    }

  
    unordered_map<string, float> widths;
    for (int j=0; j<256*256; ++j) {
        iszeroreturn = IsZeroWidth(matrixarray, j, argc-2, iszeroreturn);
        if (iszeroreturn.IsTrue) {
            widths = iszeroreturn.avg_width;
        }
      }
    for (int i=0; i <argc-2; i++){
        iszeroreturn.avg_width["glob_width"+to_string(i)] = sqrt(iszeroreturn.avg_width["glob_width"+to_string(i)] / (nohits-1));
    }
    widths = iszeroreturn.avg_width;


    // === Calculate optimal trim ===
    string name_mask = prefix + "_Matrix_Mask.csv";
    FILE *file_mask = fopen(name_mask.c_str(), "w");
    string name_trim = prefix + "_Matrix_Trim.csv";
    FILE *file_trim = fopen(name_trim.c_str(), "w");
    string name_pred = prefix + "_TrimBest_Noise_Predict.csv";
    FILE *file_pred = fopen(name_pred.c_str(), "w");
    
    
    float trim_scale;
    int trim;
    int mask;
    int predict[256*256];
    vector<int> Trims;
    float mean_widths = 0;
    int diff;
    long nmasked = 0;
    int achieved_mean = 0;
    float achieved_width = 0;
    MaskingType masking;


    
    for (int i=0; i<256*256; ++i) {
        mask = 0;
        trim_scale = Scale2Trims(inputlevels, trimvec, i, matrixarray, argc-2);
        trim = round((target - matrixarray[0][i])/trim_scale);
        predict[i] = matrixarray[0][i] + round(trim*trim_scale);
        diff = fabs(predict[i] - target);
        
        // === determine which pixels to mask
        masking = MaskingIsZero(matrixarray, i, trim, diff, dacRange, masking);
        mask = masking.mask;
        nmasked = masking.masked;
        
        if (masking.NotMasked){
            achieved_mean+=predict[i];
        }
        // Save results
        if (i%256==255) {
          fprintf(file_mask, "%d\n", mask);
          fprintf(file_trim, "%d\n", trim);
          fprintf(file_pred, "%04d\n", predict[i]);
        } else {
          fprintf(file_mask, "%d,", mask);
          fprintf(file_trim, "%d,", trim);
          fprintf(file_pred, "%04d, ", predict[i]);

        }
    }
    achieved_mean /= (256*256-nmasked);
    fclose(file_mask);
    fclose(file_trim);
    fclose(file_pred);
    
    
    for (int i=0; i<256*256; ++i) {
        if (predict[i]>0) achieved_width += pow(predict[i] - achieved_mean, 2);
    }
    achieved_width = sqrt(achieved_width/(256*256-nmasked-1));

    for (int i=0; i<argc-2; i++ ){
        cout << "  Trim" + trimvec[i] << " distribution:   " << round(means["glob_mean" + to_string(i)]) << " +/- " << round(widths["glob_width"+to_string(i)]) << endl;
    }
    cout << "\n\n";

    for (int i=0;i<argc-2;i++){
        mean_widths += widths["glob_width"+to_string(i)]/(argc-2);
    }
    
    char mean_w[24]; // dummy size, you should take care of the size!
    sprintf(mean_w, "%.2f", mean_widths);
    
    
    cout << "  Mean of widths: " << mean_w << endl;
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
