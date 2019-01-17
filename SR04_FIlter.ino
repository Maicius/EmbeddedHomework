#include "SR04.h"
#define TRIG_PIN 2
#define ECHO_PIN 3
#define MAX_K 99
#define DEFAULT_DELAY 10
#define DEFAULT_PINGS 5
#define DEFAULT_COUNT 9
SR04 sr04 = SR04(ECHO_PIN, TRIG_PIN);
long raw_val;

long median_val;
int median = 0;
long median_distance;
long distance_list[MAX_K];

long avg_val;
long avg_distance;
long avgDistanceList[MAX_K];

long median_filter(int k = 11);
long avg_filter(int delay_time = DEFAULT_DELAY, int pings = DEFAULT_PINGS);
long median_avg_filter(int k = 11);

long median_avg_val;
long median_avg_distance;
long avg_distance_list[MAX_K];
void setup() {
  Serial.begin(9600);
  delay(10);
}

int loop_count = 0;
int length_list;
int k;
void loop() {
  raw_val = sr04.Distance();
  
  if (loop_count > DEFAULT_COUNT) {
    k = loop_count % DEFAULT_COUNT;
    length_list = DEFAULT_COUNT;
  } else {
    length_list = loop_count;
  }
  median_distance = median_filter(raw_val, length_list, k);
  avg_distance = avg_filter(raw_val, length_list, k);
  median_avg_distance = median_avg_filter(raw_val, length_list, k);
  Serial.print(raw_val);
  Serial.print(',');
  Serial.print(median_distance);
  Serial.print(',');
  Serial.print(avg_distance);
  Serial.print(',');
  Serial.println(median_avg_distance);
  loop_count++;
  delay(100);
}


long median_filter(long raw_val,int length_list,int k) {

  distance_list[k] = raw_val;
  // 插入排序
  long distance_sort_list[DEFAULT_COUNT];
  for (int i = 0; i < length_list; i++) {
    median_val = distance_list[i];
    int j = i;
    while (median_val < distance_sort_list[j - 1] && j > 0) {
      distance_sort_list[j] = distance_sort_list[j - 1];
      j--;
    }
    distance_sort_list[j] = median_val;
  }

  median = k / 2;
  format_output("中位值滤波：", distance_sort_list[median]);
  return distance_sort_list[median];
}

// 均值滤波
long avg_filter(long raw_val,int length_list,int k) {
  
  avgDistanceList[k] = raw_val;
  long sum = 0;
  for (int i=0; i< length_list; i++){
      sum += avgDistanceList[i];
  }
  avg_val = sum / length_list;
  format_output("均值滤波：", avg_val);
  return avg_val;
}

// 中位值平均滤波
long median_avg_filter(long raw_val,int length_list,int k) {

  avg_distance_list[k] = raw_val;
  
  for (int i = 0; i < length_list; i++) {
    median_avg_val = avg_distance_list[i];
    int j = i;
    while (median_avg_val < avg_distance_list[j - 1] && j > 0) {
      avg_distance_list[j] = avg_distance_list[j - 1];
      j--;
    }
    avg_distance_list[j] = median_avg_val;
  }
  long sum = 0;
  for (int i = 1; i < k - 1; i++) {
    sum += avg_distance_list[i];
  }
  median_avg_val = int(sum / (k - 2));

  return median_avg_val;
}

void format_output(const char* name, long val) {
  Serial.print(name);
  Serial.print(val);
  Serial.println("cm");
}
