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


struct DataStructure{
    int counter = 0;
    int naming = 0;
    bool IsTrue;
    int NoHits=0;
    unordered_map<string, float> avg;
    unordered_map<string, float> avg_width;
};
struct MaskingStructure{
    int mask;
    long masked;
    // NoArg for checking if pixels are being masked for multiple reasons
    int NoArg;
    bool NotMasked;
    int trim;
    int predict[256*256];
};
struct TrimPredictionStructure{
    int down;
    int trim;
    int predict;
    int up;
    string pos;
    int sum;
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
        if (counter!=0){
            pixels++;
        }
        counter=0;
    }
    return pixels;
}

DataStructure CalculateMeans(uint16_t dat[10][256*256], int NoPixels, int count, DataStructure Results){
    
    for (int j=0; j<NoPixels; j++){
        for (int i=0; i<count; i++){
            Results.naming = i;
            if (dat[i][j]>0)
            {
                Results.counter +=1;
            }
            if (Results.counter == count){
                for (int k=0; k<count; k++){
                Results.avg["glob_mean"+to_string(k)] += dat[k][j];
                }
            }
        }
        if (Results.counter == count){
            Results.counter = 0;
            Results.naming = 0;
            Results.IsTrue = true;
        }
        else if (Results.counter != count){
            Results.counter = 0;
            Results.naming = 0;
            Results.IsTrue = false;
        }
        if (Results.IsTrue == true){
            Results.NoHits++;
        }
    }
    for (int i=0; i<count; i++){
        Results.avg["glob_mean" + to_string(i)] /=Results.NoHits;
    }
        return Results;
}

DataStructure CalculateWidths(uint16_t dat[10][256*256], int NoPixels, int count, DataStructure Results){
    for (int j=0; j<NoPixels; j++){
        for (int i=0; i<count; i++){
            Results.naming = i;
            if (dat[i][j]>0)
            {
                Results.counter +=1;
            }
            if (Results.counter == count){
                for (int k=0; k<count; k++){
                    Results.avg_width["glob_width" + to_string(k)] = Results.avg_width["glob_width" + to_string(k)] + pow(Results.avg["glob_mean"+to_string(k)] - dat[k][j],2);
                }
            }
        }
        if (Results.counter == count){
            Results.counter = 0;
            Results.naming = 0;
            Results.IsTrue = true;
        }
        else if (Results.counter != count){
            Results.counter = 0;
            Results.naming = 0;
            Results.IsTrue = false;
        }
    }
    for (int i=0; i<count; i++){
        Results.avg_width["glob_width"+to_string(i)] = sqrt(Results.avg_width["glob_width"+to_string(i)] / (Results.NoHits-1));
    }
    return Results;
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


MaskingStructure CalculateMasking(uint16_t dat[10][256*256], int iter, int trim, int diff, int dac, MaskingStructure ret){
    ret.mask = 0;
    ret.NoArg = 0;
    ret.NotMasked=false;
    

    if (dat[0][iter]==0 or dat[1][iter]==0 or diff>dac or trim>15 or trim<0){
        ret.masked++;
    }
    else {
        ret.NotMasked = true;
    }
    
    if (dat[0][iter]==0 && dat[1][iter]==0){
        ret.mask = 1;
        ret.NoArg++;
    }
    else if (dat[0][iter]==0){
        ret.mask = 2;
        ret.NoArg++;
    }
    else if (dat[1][iter]==0){
        ret.mask = 3;
        ret.NoArg++;
    }
    else if (trim>15 || trim<0){
        ret.mask = 4;
        ret.NoArg++;
    }
    else if (diff>dac){
        ret.mask=5;
        ret.NoArg++;
    }
    if (!ret.NotMasked){
        ret.trim = 0;
        ret.predict[iter] = 0;
    }
    return ret;
}

float Scale2Trims(unordered_map<string,float> levels, vector<string> trimvec, int iter, uint16_t dat[10][256*256], int mode = 0){
    float trimscale=0;
    if (mode==0){
        trimscale = dat[1][iter]-dat[0][iter];
        trimscale = trimscale/(levels[trimvec[1]]-levels[trimvec[0]]+1);
        return trimscale;
    }
    else if (mode==1){
        trimscale = 0.02758*pow(levels[trimvec[1]],3) - 0.6792*pow(levels[trimvec[1]],2) + 19.48*levels[trimvec[1]] + 1296 - (0.02758*pow(levels[trimvec[0]],3) - 0.6792*pow(levels[trimvec[0]],2) + 19.48*levels[trimvec[0]] + 1296);
        // not sure if this is correct
        if (std::count(trimvec.begin(), trimvec.end(), "0")) {
            trimscale = trimscale/(levels[trimvec[1]] - levels[trimvec[0]]+1);
        }
        else{
            trimscale = trimscale/(levels[trimvec[1]] - levels[trimvec[0]]);

        }
        return trimscale;
    }
    else {
        cout << "Trimscale calculation failed" << endl;
        trimscale = 0;
        return trimscale;
    }
    
}

float TrimScaleStep(int start, int end){
    float trimscale = 0;
    trimscale = 0.02758*pow(end,3) - 0.6792*pow(end,2) + 19.48*end + 1296 - (0.02758*pow(start,3) - 0.6792*pow(start,2) + 19.48*start + 1296);
    return trimscale;
}


TrimPredictionStructure GoUp(uint16_t dat[256*256], TrimPredictionStructure trimpredict, int target, int start, int stop, int iter){
    trimpredict.down = 0;
    trimpredict.sum = 0;
    int diff1 = fabs(target - dat[iter]);
    int i=start;
    while (diff1>round(TrimScaleStep(7, 8)/2)){
        //  if first given trimlevel=0, it cannot be shifted en therefore the loop must end. However, a trim should be calculated
        if (i+1>stop){
            // should not happen since it would be stoped by last time/with residual (except when first trim equals 0)
            trimpredict.up+=round(diff1/TrimScaleStep(i-1, i));
            trimpredict.sum += diff1;
            break;
        }
        // last time/no residual
        if (diff1-TrimScaleStep(i, i+1)*1 <= round(TrimScaleStep(i, i+1)/2) and i+1<=stop){
            diff1 = diff1-TrimScaleStep(i, i+1)*1;
            trimpredict.sum +=TrimScaleStep(i, i+1)*1 ;
            trimpredict.up++;
            i++ ;
            break;
        }
        // no levels remaining but we arent there yet
        else if (diff1-TrimScaleStep(i, i+1)*1 > round(TrimScaleStep(i, i+1)/2) and i+1==stop){
            trimpredict.sum +=diff1;
            trimpredict.up++;
            break;
        }
        // last time/with residual
        else if (diff1-TrimScaleStep(i, i+1)*1 > round(TrimScaleStep(0, 1)/2) and i+1==stop){
            diff1 = diff1-TrimScaleStep(i, i+1)*1;
            trimpredict.sum +=diff1;
            trimpredict.up++;
            trimpredict.up+=round(diff1/TrimScaleStep(14, 15));
            i++;
            break;
        }
        // keep going/levels remaining
        else if (diff1-TrimScaleStep(i, i+1)*1 > round(TrimScaleStep(i, i+1)/2) and i+1<stop){
            diff1 = diff1-TrimScaleStep(i, i+1)*1;
            trimpredict.sum+=TrimScaleStep(i, i+1)*1;
            trimpredict.up++;
            i++;
        }
    }
    trimpredict.trim = trimpredict.up;
    return trimpredict;
}

TrimPredictionStructure GoDown(uint16_t dat[256*256], TrimPredictionStructure trimpredict, int target, int start, int iter){
    trimpredict.up = 0;
    trimpredict.sum = 0;
    int diff1 = fabs(target - dat[iter]);
    int i=start;
    while (diff1>round(TrimScaleStep(0, 1)/2)){
        //  if first given trimlevel=0, it cannot be shifted en therefore the loop must end. However, a trim should be calculated
        if (i-1<0){
            // should not happen since it would be stoped by last time/with residual (except when first trim equals 0)
            // should include residual since we know that when it comes here trim0 = 0 and it is not within the border
            trimpredict.down+=round(diff1/TrimScaleStep(0, 1));
            trimpredict.sum = diff1;
            break;
        }
        // last time/no residual
        if (diff1-TrimScaleStep(i-1, i)*1 <= round(TrimScaleStep(i-1, i)/2) and i-1>=0){
            diff1 = diff1-TrimScaleStep(i-1, i)*1;
            trimpredict.sum =diff1;
            trimpredict.down++;
            i--;
            break;
        }
        // last time/with residual
        else if (diff1-TrimScaleStep(i-1, i)*1 > round(TrimScaleStep(0, 1)/2) and i-1==0){
            diff1 = diff1-TrimScaleStep(i-1, i)*1;
            trimpredict.sum =diff1;
            trimpredict.down++;
            trimpredict.down+=round(diff1/TrimScaleStep(0, 1));
            break;
        }
        // keep going/levels remaining
        else if (diff1-TrimScaleStep(i-1, i)*1 > round(TrimScaleStep(i-1, i)/2)  and i-1>0){
            diff1 = diff1-TrimScaleStep(i-1, i)*1;
            trimpredict.sum =TrimScaleStep(i-1, i)*1;
            trimpredict.down++;
            i--;
        }
    }
    trimpredict.trim = -trimpredict.down;
    
    
    return trimpredict;
}


TrimPredictionStructure CalcTrim(int target, uint16_t dat[10][256*256], int iter, float trimscale, vector<string> trimvec, unordered_map<string,float> inputlevels, TrimPredictionStructure trimpredict, int mode = 0){
    trimpredict.trim = 0;
    trimpredict.up=0;
    trimpredict.down = 0;
    if (mode==0){
        trimpredict.trim = round((target - dat[0][iter])/trimscale);
    }
    else if (mode==1){
        if ((target - dat[0][iter])>0 and target - dat[1][iter] < 0){
            trimpredict.pos = "between";
            trimpredict = GoUp(dat[0], trimpredict, target, inputlevels[trimvec[0]], inputlevels[trimvec[1]], iter);
        }
        else if (target - dat[0][iter]<0){
            trimpredict.pos = "below";
            trimpredict = GoDown(dat[0], trimpredict, target, 0, iter);
        }
        else if (target - dat[1][iter]>0){
            trimpredict.pos = "over";
            trimpredict = GoUp(dat[1], trimpredict, target, inputlevels[trimvec[1]], 15, iter);
        }
        else if ((target - dat[0][iter])==0){
            trimpredict.trim=0;
            trimpredict.trim=inputlevels[trimvec[0]];
            trimpredict.pos = "equal1";
//            cout << "equal1" << endl;
        }
        else if ((target - dat[1][iter])==0){
            trimpredict.trim=inputlevels[trimvec[1]];
            trimpredict.pos = "equal2";
//            cout << "equal2" << endl;
        }
    }
    return trimpredict;
}


int CalcPredict(uint16_t dat[10][256*256], int iter, TrimPredictionStructure trimpredict, float trimscale, int mode = 0){
    int predict=0;
    if (mode==0){
        predict = dat[0][iter] + round(trimpredict.trim*trimscale);
    }
    else if (mode==1){
        if (trimpredict.pos == "over"){
            predict = dat[1][iter] + trimpredict.sum;

        }
        else if (trimpredict.pos=="between"){
            predict = dat[0][iter] + trimpredict.sum;
        }
        else if (trimpredict.pos == "below"){
            predict = dat[0][iter] - trimpredict.sum;

        }
        else if (trimpredict.pos == "equal1"){
            predict = dat[0][iter];
        }
        else if (trimpredict.pos == "equal2"){
            predict = dat[1][iter];
        }
    }
    else if (mode==2){
        if (trimpredict.pos == "over"){
            predict = dat[1][iter] + round(trimpredict.up*trimscale);

        }
        else if (trimpredict.pos=="between"){
            predict = dat[0][iter] + round(trimpredict.up*trimscale);
        }
        else if (trimpredict.pos == "below"){
            predict = dat[0][iter] - round(trimpredict.down*trimscale);

        }
        else if (trimpredict.pos == "equal1"){
            predict = dat[0][iter];
        }
        else if (trimpredict.pos == "equal2"){
            predict = dat[1][iter];
        }
    }
    return predict;
}

int CalcDiff(int target, int predict){
    int diff = 0;
    diff = fabs(target-predict);
    return diff;
}

float CalcAchievWidth(int predict[256*256], int NoPixels, float achieved_mean, int nmasked){
    float achieved_width=0;
    if (achieved_mean==0){
        cout << "[dim_equalisation.CalcAchievWidth]: Failed. Achieved_mean=0" << endl;
    }
    for (int i =0; i<NoPixels; i++){
        if (predict[i]>0){
            achieved_width += pow(predict[i]-achieved_mean, 2);
        }
    }
    achieved_width = sqrt(achieved_width/(NoPixels-nmasked-1));
    return achieved_width;
}

float CalculateTarget(unordered_map<string, float> means, vector<string> trimvec, unordered_map<string, float> levels){
    float target = 0;
    float mid = 7.5;
    float w1 = (mid-levels[trimvec[0]])/16;
    float w2 = (levels[trimvec[1]]-mid)/16;
    target = ((1/w1)*means["glob_mean"+to_string(0)] + (1/w2)*means["glob_mean"+to_string(1)])/((w1+w2)/(w1*w2));
    return target;
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
    
    // === Initializing vector to track trim levels === //
    vector<string> trimvec(10);
    for (int i=2; i<argc; i++){
        trimvec[i-2] = argv[i];
    }
    
    // === Translate from hexadecimal to decimal
    unordered_map<string,float> inputlevels;
    inputlevels = TrimValues(trimvec, inputlevels, argc-2);

    
    int dacRange = 25; // Tuneable parameter
    
    // === load means ===
    uint16_t matrixarray[10][256*256];
    for (int i=0; i<argc-2; i++)
    {
        load_mean(prefix+"_Trim"+argv[i+2]+"_Noise_Mean.csv", matrixarray[i]);
    }
    
    //  === Hard coding the 0 trim for target ===
    load_mean(prefix+"_Trim0_Noise_Mean.csv", matrixarray[argc-2]);
    load_mean(prefix+"_TrimF_Noise_Mean.csv", matrixarray[argc-1]);
    
    //  === Calculate pixels that are lost since they do not respond (mean_trimi==0)
    int mask_pixels_init = 0;
    mask_pixels_init = NoOfMaskedPixels(matrixarray, argc-2);
    cout << "  Number of masked pixels initially: " << mask_pixels_init << endl;

    
    // === Calculate Target ===
    cout << "  [dim_equalisation] Equalising" << endl;
    
    // === calculate averages
    unordered_map<string, float> means;
    DataStructure Results;
    Results = CalculateMeans(matrixarray, 256*256, argc-2, Results);
    means = Results.avg;

    // === hardcoding mean of 0 and f trim for hardcoded target ===
    for (int i =0; i<256*256; i++){
        means["glob_mean"+to_string(argc-2)]+=matrixarray[argc-2][i];
        means["glob_mean"+to_string(argc-1)]+=matrixarray[argc-1][i];
    }
    means["glob_mean"+to_string(argc-2)]/=Results.NoHits;
    means["glob_mean"+to_string(argc-1)]/=Results.NoHits;

    
    
//  ======================= Target  =============================
    
//  =================== Target function =========================
    float target = 0;
    target = round(CalculateTarget(means, trimvec, inputlevels));
    
//  =================== Hardcoded target =========================
    int hctarget = 0;
    hctarget = (means["glob_mean"+to_string(argc-2)] + means["glob_mean"+to_string(argc-1)])/2;

//  ============================================================
    

    if (Results.NoHits==0) {
        cout << "[dim_equalisation] FAILED: Threshold scan has empty output file" << endl;
        return 0;
    }

    // === calculate widths of distributions ===
    unordered_map<string, float> widths;
    Results = CalculateWidths(matrixarray, 256*256, argc-2, Results);
    widths = Results.avg_width;
    
    
    // === Calculate optimal trim ===
    string name_mask = prefix + "_Matrix_Mask"+argv[2]+argv[3]+".csv";
    FILE *file_mask = fopen(name_mask.c_str(), "w");
    string name_trim = prefix + "_Matrix_Trim"+argv[2]+argv[3]+".csv";
    FILE *file_trim = fopen(name_trim.c_str(), "w");
    string name_pred = prefix + "_TrimBest"+argv[2]+argv[3]+"_Noise_Predict.csv";
    FILE *file_pred = fopen(name_pred.c_str(), "w");
    
    
    float trim_scale;
    int mask;
    vector<int> Trims;
    float mean_widths = 0;
    int diff;
    long nmasked = 0;
    int achieved_mean = 0;
    float achieved_width = 0;
    MaskingStructure masking;
    TrimPredictionStructure trim_predict;


//  =================== Masking and prediction =========================
    for (int i=0; i<256*256; ++i) {
        mask = 0;
        //  Last argument of Scale2Trims decides which method is used. (0 = mean of 0 and F, 1 = polynomial)
        //  Last argument of CalcTrim decides which method is used. (0 = only up and 1 = both up and down (if trim1!=0))
        trim_scale = Scale2Trims(inputlevels, trimvec, i, matrixarray, 1);
        trim_predict = CalcTrim(target, matrixarray, i, trim_scale, trimvec, inputlevels, trim_predict, 1);
        masking.predict[i] = CalcPredict(matrixarray, i, trim_predict, trim_scale, 1);

        // transform trimlevels into actual trimlevels (i.e + its starting point)
        if (trim_predict.pos=="below"){
            trim_predict.trim+=inputlevels[trimvec[0]];
        }
        else if (trim_predict.pos=="between"){
            trim_predict.trim+=inputlevels[trimvec[0]];
        }
        else if (trim_predict.pos == "over"){
            trim_predict.trim+=inputlevels[trimvec[1]];
        }
        diff = CalcDiff(target, masking.predict[i]);
        masking = CalculateMasking(matrixarray, i, trim_predict.trim, diff, dacRange, masking);
        mask = masking.mask;
        nmasked = masking.masked;
        if (masking.NotMasked){
            achieved_mean+=masking.predict[i];
        }
        // Save results
        if (i%256==255) {
          fprintf(file_mask, "%d\n", mask);
          fprintf(file_trim, "%d\n", trim_predict.trim);
          fprintf(file_pred, "%04d\n", masking.predict[i]);
        } else {
          fprintf(file_mask, "%d,", mask);
          fprintf(file_trim, "%d,", trim_predict.trim);
          fprintf(file_pred, "%04d, ", masking.predict[i]);

        }
    }
    achieved_mean /= (256*256-nmasked);
    fclose(file_mask);
    fclose(file_trim);
    fclose(file_pred);
    achieved_width = CalcAchievWidth(masking.predict, 256*256, achieved_mean, nmasked);
    
    cout << "  [dim_equalisation] Summary" << endl;

    for (int i=0; i<argc-2; i++ ){
        cout << "  Trim " + trimvec[i] << " distribution:  " << round(means["glob_mean" + to_string(i)]) << " +/- " << round(widths["glob_width"+to_string(i)]) << endl;
    }
    
    char mean_w[24]; // dummy size, you should take care of the size!
    sprintf(mean_w, "%.2f", mean_widths);
    
//  ====================== Summary ============================
    cout << "  Equalisation Target:  " << target << endl;
    cout << "  Hardcoded target:  " << hctarget << endl;
    cout << "  Difference hctarget and Target:  " << fabs(hctarget-target) << endl;
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
  fclose(file_tp);
  
  return 0;
}
