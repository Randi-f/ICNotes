/*
 * @Author: shihan
 * @Date: 2024-04-28 17:26:54
 * @version: 1.0
 * @description: 
 */
#include<iostream>
using namespace std;
void quick_sort(int list[], int left, int right){
    int pivot, left_arrow, right_arrow;
    left_arrow = left;
    right_arrow = right;
    pivot = (left+right)/2;
    do{
        while(list[right_arrow]>pivot)
            right_arrow--;
        while(list[left_arrow]<pivot)
            left_arrow++;
        if(left_arrow<=right_arrow){
            swap(list[left_arrow],list[right_arrow]);
            left_arrow++;
            right_arrow--;
        }
        
    }while(right_arrow>=left_arrow);
    if(left<right_arrow){
        quick_sort(list, left, right_arrow);
    }
    if(left_arrow<right){
        quick_sort(list, left_arrow, right);
    }
}

void swap(int& a, int& b){
    int tmp = a;
    a = b;
    b = tmp;
}

int main(){
    int list[4] = {3,2,5,1};
    quick_sort(list,0,3);
    for(int i=0; i<4;i++){
        cout << list[i] << endl;
    }

}