#include<stdio.h>
#include<string.h>
struct Detail {
    int score;
    int name;
};
struct ScoreClass {
    struct Detail scoreDetail[100];
    int num;
};
int average(struct ScoreClass class) {
    int scoreSum = 0;
    int i = 0; 
    int N = class.num;
	printf("%d\n",N);
    while (i < N) {
        scoreSum = class.scoreDetail[i].score + scoreSum;
        i = i + 1;
    }
    return scoreSum / N;
}
int main() {
    struct ScoreClass classInit;
    int result = 0, initN = 10;
    int j = 0;
    classInit.num = initN;
    while (j < initN) {
        classInit.scoreDetail[j].score = (j + 1) * (j + 1);
        j = j + 1;
    }
    result = average(classInit);
    //printf("%d\n",result);
	//printf("%d\n",(int)sizeof(struct Detail));
	//printf("%d\n",(int)sizeof(struct ScoreClass));
    return 0;
}
