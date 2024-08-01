/*
 gcc -o texas_holdem_simulation texas_holdem_simulation.c
./texas_holdem_simulation

*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_CARDS 52
#define NUM_HANDS 1000000
#define HAND_SIZE 7
#define NUM_PLAYERS 10
#define HOLE_CARDS 2
#define COMMUNITY_CARDS 5

// 定義花色類型
typedef enum {
    HEARTS,    // 紅心
    DIAMONDS,  // 方塊
    CLUBS,     // 梅花
    SPADES     // 黑桃
} Suit;

// 定義卡片結構
typedef struct {
    Suit suit;   // 花色
    short pips;  // 點數
} Card;

// 洗牌函數
void shuffleDeck(Card* deck, int size) {
    for (int i = size - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        Card temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}

// 初始化牌組
void initializeDeck(Card* deck) {
    for (int i = 0; i < NUM_CARDS; ++i) {
        deck[i].suit = i / 13;
        deck[i].pips = i % 13 + 1;
    }
}

// 比較函數，用於排序
int compare(const void* a, const void* b) {
    return ((Card*)a)->pips - ((Card*)b)->pips;
}

// 評估手牌函數
void evaluateHand(Card* hand, int* counts) {
    int pipsCount[14] = { 0 };  // 計算每個點數的張數
    int suitCount[4] = { 0 };   // 計算每個花色的張數

    for (int i = 0; i < HAND_SIZE; ++i) {
        pipsCount[hand[i].pips]++;
        suitCount[hand[i].suit]++;
    }

    // 初始化各種牌型的計數
    int pairs = 0, threeOfKind = 0, fourOfKind = 0, flush = 0, straight = 0;
    int fullHouse = 0, straightFlush = 0, royalFlush = 0;

    // 計算對子、三條、四條的數量
    for (int i = 1; i <= 13; ++i) {
        if (pipsCount[i] == 2) pairs++;
        if (pipsCount[i] == 3) threeOfKind++;
        if (pipsCount[i] == 4) fourOfKind++;
    }

    // 計算同花的數量
    for (int i = 0; i < 4; ++i) {
        if (suitCount[i] >= 5) flush++;
    }

    // 計算順子的數量
    for (int i = 1; i <= 9; ++i) {
        if (pipsCount[i] && pipsCount[i + 1] && pipsCount[i + 2] && pipsCount[i + 3] && pipsCount[i + 4]) {
            straight++;
            if (flush) {
                straightFlush++;
                if (i == 1 && pipsCount[10] && pipsCount[11] && pipsCount[12] && pipsCount[13]) {
                    royalFlush++;
                }
            }
        }
    }

    // 根據牌型更新計數
    if (fourOfKind) counts[2]++;
    else if (threeOfKind && pairs) fullHouse++;
    else if (threeOfKind) counts[6]++;
    else if (pairs == 2) counts[7]++;
    else if (pairs == 1) counts[8]++;
    else counts[9]++;

    if (fullHouse) counts[3]++;
    if (flush) counts[4]++;
    if (straight) counts[5]++;
    if (straightFlush) counts[1]++;
    if (royalFlush) counts[0]++;
}

// 發牌函數
void dealHands(Card* deck, Card hands[NUM_PLAYERS][HOLE_CARDS], Card* community) {
    int cardIndex = 0;

    // 發底牌
    for (int i = 0; i < NUM_PLAYERS; ++i) {
        for (int j = 0; j < HOLE_CARDS; ++j) {
            hands[i][j] = deck[cardIndex++];
        }
    }

    // 發公共牌
    for (int i = 0; i < COMMUNITY_CARDS; ++i) {
        community[i] = deck[cardIndex++];
    }
}

// 組合手牌函數
void combineHand(Card* hand, Card* holeCards, Card* community) {
    for (int i = 0; i < HOLE_CARDS; ++i) {
        hand[i] = holeCards[i];
    }
    for (int i = 0; i < COMMUNITY_CARDS; ++i) {
        hand[HOLE_CARDS + i] = community[i];
    }
}

int main() {
    srand(time(NULL));

    Card deck[NUM_CARDS];
    Card hands[NUM_PLAYERS][HOLE_CARDS];
    Card community[COMMUNITY_CARDS];
    Card combinedHand[HAND_SIZE];
    int counts[10] = { 0 }; // 0: 皇家同花順, 1: 同花順, 2: 四條, 3: 葫蘆, 4: 同花, 5: 順子, 6: 三條, 7: 兩對, 8: 一對, 9: 高牌

    initializeDeck(deck);

    for (int i = 0; i < NUM_HANDS; ++i) {
        shuffleDeck(deck, NUM_CARDS);
        dealHands(deck, hands, community);

        for (int j = 0; j < NUM_PLAYERS; ++j) {
            combineHand(combinedHand, hands[j], community);
            qsort(combinedHand, HAND_SIZE, sizeof(Card), compare);
            evaluateHand(combinedHand, counts);
        }
    }

    int totalHands = NUM_HANDS * NUM_PLAYERS;

    // 按照圖片中的順序打印結果
    printf("Royal flush: %d (%.8f)\n", counts[0], (double)counts[0] / totalHands);
    printf("Straight flush: %d (%.8f)\n", counts[1], (double)counts[1] / totalHands);
    printf("Four of a kind: %d (%.8f)\n", counts[2], (double)counts[2] / totalHands);
    printf("Full house: %d (%.8f)\n", counts[3], (double)counts[3] / totalHands);
    printf("Flush: %d (%.8f)\n", counts[4], (double)counts[4] / totalHands);
    printf("Straight: %d (%.8f)\n", counts[5], (double)counts[5] / totalHands);
    printf("Three of a kind: %d (%.8f)\n", counts[6], (double)counts[6] / totalHands);
    printf("Two pair: %d (%.8f)\n", counts[7], (double)counts[7] / totalHands);
    printf("One pair: %d (%.8f)\n", counts[8], (double)counts[8] / totalHands);
    printf("High card: %d (%.8f)\n", counts[9], (double)counts[9] / totalHands);

    double sumProbabilities = 0.0;
    for (int i = 0; i < 10; ++i) {
        sumProbabilities += (double)counts[i] / totalHands;
    }
    printf("Total: %d\n", totalHands);
    printf("Sum of all probabilities: %.8f\n", sumProbabilities);

    return 0;
}