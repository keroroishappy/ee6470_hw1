#include <cmath>
#include "SobelFilter.h"

Filter::Filter(sc_module_name n) : sc_module(n) {
  
  SC_THREAD(do_filter);
  sensitive << i_clk.pos();
  //dont_initialize();
  // SC_THREAD(do_mean);
  // sensitive << i_clk.pos();
  SC_THREAD(do_output);
  sensitive << event1;
  //dont_initialize();
  //reset_signal_is(i_rst, false);
}


int window[9];
int row=1;
int col=1;
int i=0;
int buffer_9[3][3];
int temp_median[512][512];
int ext_temp_median[513][513]={0};

void insertionSort(int arr[], int n)
{
    int i, key, j;
    for (i = 1; i < n; i++)
    {
        key = arr[i];
        j = i - 1;
        while (j >= 0 && arr[j] > key)
        {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}


int find_median(int array[][3]){
      //neighbor pixel values are stored in window including this pixel
      int median_value=0;
      window[0] = array[row-1][col-1];
      window[1] = array[row-1][col];
      window[2] = array[row-1][col+1];
      window[3] = array[row][col-1];
      window[4] = array[row][col];
      window[5] = array[row][col+1];
      window[6] = array[row+1][col-1];
      window[7] = array[row+1][col];
      window[8] = array[row+1][col+1];
      insertionSort(window,9);
      median_value=window[4];  
      return median_value;
}


void ext_arr(void){
  for(int row=1; row<512; row++){
    for(int col=1; col<512; col++){
      ext_temp_median[row][col]=temp_median[row-1][col-1];  
    }
  }
}

void do_mean(void){
  ext_arr();
  for(int row=1; row<512; row++){
    for(int col=1; col<512; col++){
      window[0] = ext_temp_median[row-1][col-1];
      window[1] = ext_temp_median[row-1][col];
      window[2] = ext_temp_median[row-1][col+1];
      window[3] = ext_temp_median[row][col-1];
      window[4] = 2*(ext_temp_median[row][col]);
      window[5] = ext_temp_median[row][col+1];
      window[6] = ext_temp_median[row+1][col-1];
      window[7] = ext_temp_median[row+1][col];
      window[8] = ext_temp_median[row+1][col+1];
      int mean=window[0]+window[1]+window[2]+window[3]+window[4]+window[5]+window[6]+window[7]+window[8];
      mean=mean/10;
      //std::cout<<"mean= "<<mean<<"  "<<std::endl;
      temp_median[row-1][col-1]=mean;

    }
  }
}


void Filter::do_output(void){
  while(true){
    //wait(event1);
    do_mean();
    std::cout<<"execute do output"<<std::endl;
    int val2;
    for(int row=1; row<512; row++){
      for(int col=1; col<512; col++){
      val2=temp_median[row][col]; 
      int result=(int)(val2);
      std::cout<<"result= "<<result<<"  "<<std::endl;
      o_result.write(result);
      }
    }
      wait(1,SC_NS);
  }
}



void Filter::do_filter(){
  int grey;
  int x=0;
  int i=0;
  int y=0;

  while (true) {
    //median filter
    int median_value=0;
    for ( int v = 0; v < MASK_Y; ++v) {
      for ( int u = 0; u < MASK_X; ++u) {
        grey = (i_r.read() + i_g.read() + i_b.read()) / 3; //convert each pixel to greyscale
        //x=x+1;
        //std::cout<<grey<<std::endl;
        buffer_9[v][u]=grey;
      }
    }
    median_value=find_median(buffer_9);
    //std::cout<<"medain value  "<<median_value<<"  "<<std::endl;

    if(y==512){
      x=x+1;
      y=0;
    }
    
    temp_median[x][y]=median_value;
    //std::cout<<"x value= "<<x<<std::endl;
    //std::cout<<"y value= "<<y<<std::endl;
    y=y+1;

    //val2=temp_median[row][col]; 
    
    //int result=(int)(median_value);
    //std::cout<<"result= "<<result<<"  "<<std::endl;
    //o_result.write(result);
    i=i+1;
    std::cout<<"i= "<<i<<std::endl;
    if(i%5==1){
      event1.notify();
      std::cout<<"notify_is_trigger"<<std::endl;
    }
    wait(1,SC_NS);
  }
}


