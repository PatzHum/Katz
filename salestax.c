#define SALES_TAX 1.13

#include <stdio.h>
#include <stdlib.h>

void addTax(float &price);

int main(){
    float price = 0;

    printf("Enter price:");
    scanf("%f", &price);

    addTax(price);

    printf("\nPrice with tax: %f", price);
}

void addTax(float &price){
    price *= SALES_TAX;
}
