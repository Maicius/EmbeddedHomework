#include "SR04.h"
#define TRIG_PIN 2
#define ECHO_PIN 3
#define MAX_K 99
#define DEFAULT_DELAY 10
#define DEFAULT_PINGS 5
SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN);
long raw_val;

long median_val;
int median = 0;
long median_distance;
long distance_list[MAX_K];

long avg_val;
long avg_distance;

long median_filter(int k = 11);
long avg_filter(int delay_time=DEFAULT_DELAY, int pings=DEFAULT_PINGS);
long median_avg_filter(int k = 11);

long median_avg_val;
long median_avg_distance;
long avg_distance_list[MAX_K];
void setup() {
   Serial.begin(9600);
   delay(10);
}

void loop() {
   raw_val = sr04.Distance();
   median_distance = median_filter(11);
   avg_distance = avg_filter();
   median_avg_distance = median_avg_filter();
   Serial.print(raw_val);
   Serial.print(',');
   Serial.print(median_distance);
   Serial.print(',');
   Serial.print(avg_distance);
   Serial.print(',');
   Serial.println(median_avg_distance);
   delay(10);
}


long median_filter(int k = 11) {
    for(int i = 0; i < k; i++){
      median_val = sr04.Distance();
      int j = i;
      while(median_val < distance_list[j - 1] && j > 0){
        distance_list[j] = distance_list[j - 1];
        j--;
      }
      distance_list[j] = median_val;
    }
//    for(int i=0; i < k; i++){
//      Serial.print(distance_list[i]);
//      Serial.print(" ");
//    }
    median = k / 2;
    format_output("中位值滤波：", distance_list[median]);
    return distance_list[median];
}

long avg_filter(int delay_time=DEFAULT_DELAY, int pings=DEFAULT_PINGS) {
    avg_val = sr04.DistanceAvg(delay_time, pings);
    format_output("均值滤波：", avg_val);
    return avg_val;
}

long median_avg_filter(int k = 11) {
    for(int i = 0; i < k; i++){
      median_avg_val = sr04.Distance();
      int j = i;
      while(median_avg_val < avg_distance_list[j - 1] && j > 0){
        avg_distance_list[j] = avg_distance_list[j - 1];
        j--;
      }
      avg_distance_list[j] = median_avg_val;
    }
    long sum = 0;
    for(int i = 1; i < k - 1; i++){
      sum += avg_distance_list[i];
    }
    median_avg_val = int(sum / (k - 2));

    return median_avg_val;
}

void format_output(const char* name, long val){
  Serial.print(name);
  Serial.print(val);
  Serial.println("cm");
}
