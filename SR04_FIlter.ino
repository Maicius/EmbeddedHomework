#include "SR04.h"
#define TRIG_PIN 2
#define ECHO_PIN 3
#define MAX_K 99
#define DEFAULT_DELAY 10
#define DEFAULT_PINGS 5
#define DEFAULT_COUNT 9
SR04 sr04 = SR04(ECHO_PIN, TRIG_PIN);
float raw_val;

float median_val;
int median = 0;
float median_distance;
float distance_list[MAX_K];

float avg_val;
float avg_distance;
float avgDistanceList[MAX_K];

float median_filter(float raw_val, int length_list, int k);
float avg_filter(float raw_val, int length_list, int k);
float median_avg_filter(float raw_val, int length_list, int k);
float getDistance();
float median_avg_val;
float median_avg_distance;
float avg_distance_list[MAX_K];
void setup() {
  Serial.begin(9600);
  delay(10);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

int loop_count = 0;
int length_list;
int k;
void loop() {
  //官方库，获取的值为long int型
//  raw_val = sr04.Distance();
  //  自定义方法，获取的值为float型，更精确
  raw_val = getDistance();
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
  Serial.print(median_avg_distance);
  Serial.print(',');
  Serial.println(100);
  loop_count++;
  delay(100);
}

// 中位值滤波
float median_filter(float raw_val, int length_list, int k) {

  distance_list[k] = raw_val;
  // 插入排序
  float distance_sort_list[DEFAULT_COUNT];
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
float avg_filter(float raw_val, int length_list, int k) {

  avgDistanceList[k] = raw_val;
  float sum = 0;
  for (int i = 0; i < length_list; i++) {
    sum += avgDistanceList[i];
  }
  avg_val = sum / length_list;
  format_output("均值滤波：", avg_val);
  return avg_val;
}

// 中位值平均滤波
float median_avg_filter(float raw_val, int length_list, int k) {
  avg_distance_list[k] = raw_val;
  float distance_sort_list_2[DEFAULT_COUNT];
  for (int i = 0; i < length_list; i++) {
    median_avg_val = avg_distance_list[i];
    int j = i;
    while (median_avg_val < distance_sort_list_2[j - 1] && j > 0) {
      distance_sort_list_2[j] = distance_sort_list_2[j - 1];
      j--;
    }
    distance_sort_list_2[j] = median_avg_val;
  }

  if (length_list > 2) {
    float sum = 0;
    for (int i = 1; i < length_list - 1; i++) {
      sum += distance_sort_list_2[i];
    }
    median_avg_val = float(sum / (length_list - 2));
  }
  else {
    median_avg_val = avg_distance_list[k];
  }
  format_output("中位值均值滤波：", median_avg_val);
  return median_avg_val;
}

void format_output(const char* name, float val) {
  Serial.print(name);
  Serial.print(val);
  Serial.println("cm");
}

float getDistance() {
  float cm;
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  cm = pulseIn(ECHO_PIN, HIGH) / 58.82; //算成厘米
  cm = (int(cm * 100.0)) / 100.0; //保留两位小数
  return cm;
}
