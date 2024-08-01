#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#define NUM_CARDS 52
//#define NUM_HANDS 1000000
#define NUM_HANDS 13378456
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
int compareByPips(const void* a, const void* b) {
    const Card* cardA = (const Card*)a;
    const Card* cardB = (const Card*)b;

    // 檢查點數是否為 -1
    if (cardA->pips == -1 && cardB->pips == -1) return 0;
    if (cardA->pips == -1) return -1;
    if (cardB->pips == -1) return 1;

    // 點數比較
    return cardA->pips - cardB->pips;
}

//// 比較函數，用於按花色和點數排序
//int compareBySuitAndPips(const void* a, const void* b) {
//    Card* cardA = (Card*)a;
//    Card* cardB = (Card*)b;
//    if (cardA->suit != cardB->suit) {
//        return cardA->suit - cardB->suit;
//    }
//    else {
//        return cardA->pips - cardB->pips;
//    }
//}

// 評估手牌函數
// 函數檢查 suitCount 是否有數字大於等於 5
bool hasFlush(int* suitCount) {
    for (int i = 0; i < 4; ++i) {
        if (suitCount[i] >= 5) {
            return true;
        }
        //if (suitCount[i] == 7) {
        //    return true;
        //}
        //else if (suitCount[i] == 6) {
        //    return true;
        //}
        //else if (suitCount[i] == 5) {
        //    return true;
        //}

    }
    return false;
}
//當有suit超過五個時回傳花色
int getFlushSuit(int* suitCount) {
    for (int i = 0; i < 4; ++i) {
        if (suitCount[i] >= 5) {
            return i;
        }
    }
    return -1;
}
//函數將 handBySuit 中與指定 suit 不同的牌變為 suit = -1 和 pips = -1
void filterHandBySuit(int suit, Card* handBySuit) {
    for (int i = 0; i < HAND_SIZE; ++i) {
        if (handBySuit[i].suit != suit) {
            handBySuit[i].suit = -1;
            handBySuit[i].pips = -1;
        }
    }
}
// 函數檢查 handBySuit 是否包含點數為 10、11、12、13 和 1 的牌
bool hasRoyalFlush(Card* handBySuit) {
    bool has10 = false, has11 = false, has12 = false, has13 = false, has1 = false;

    for (int i = 0; i < HAND_SIZE; ++i) {
        if (handBySuit[i].pips == 10) has10 = true;
        if (handBySuit[i].pips == 11) has11 = true;
        if (handBySuit[i].pips == 12) has12 = true;
        if (handBySuit[i].pips == 13) has13 = true;
        if (handBySuit[i].pips == 1) has1 = true;
    }

    return has10 && has11 && has12 && has13 && has1;
}
// 函數檢查 handByPips 是否包含順子
bool hasStraight(Card* handByPips) {
    // 創建一個新的數組來存儲已經排序的點數
    int pips[HAND_SIZE];
    for (int i = 0; i < HAND_SIZE; ++i) {
        pips[i] = handByPips[i].pips;
    }

    // 對點數進行排序，將 -1 排到最前面
    // /////////qsort(pips, HAND_SIZE, sizeof(int), compareByPips);

    // 檢查是否有五張連續的牌
    // 檢查是否有五張67連續的牌
    int consecutive = 1;
    for (int i = 1; i < HAND_SIZE; ++i) {
        if (pips[i] == -1) continue; // 忽略 -1 的值
        if (pips[i] == pips[i - 1] + 1) {
            consecutive++;
            //if (consecutive == 5) {
            /*return true;
            }*/
            if (consecutive == 7) {
                return true;
            }
            else if (consecutive == 6) {
                return true;
            }
            else if (consecutive == 5) {
                return true;
            }
        }
        else if (pips[i] != pips[i - 1]) {
            consecutive = 1;
        }
    }

    // 檢查特例：A, 2, 3, 4, 5
    if (pips[0] == 1 && pips[1] == 2 && pips[2] == 3 && pips[3] == 4 && pips[4] == 5) {
        return true;
    }

    return false;
}

void countCardPips(Card hand[], int size, int matches[]) {
    int pipsCount[14] = { 0 };  // 點數範圍是 1 到 13，忽略 0

    // 統計每個點數出現的次數
    for (int i = 0; i < size; i++) {
        if (hand[i].pips > 0 && hand[i].pips <= 13) {
            pipsCount[hand[i].pips]++;
        }
    }

    // 統計結果
    matches[0] = 0; // 四張點數相同的牌
    matches[1] = 0; // 三張點數相同的牌
    matches[2] = 0; // 兩張點數相同的牌

    for (int i = 1; i <= 13; i++) {
        if (pipsCount[i] == 4) {
            matches[0]++;
        }
        else if (pipsCount[i] == 3) {
            matches[1]++;
        }
        else if (pipsCount[i] == 2) {
            matches[2]++;
        }
    }
}
// 評估手牌函數
void evaluateHand(Card* handByPips, int* counts) {
    int pipsCount[14] = { 0 };  // 計算每個點數的張數
    int suitCount[4] = { 0 };   // 計算每個花色的張數
    for (int i = 0; i < HAND_SIZE; ++i) {
        pipsCount[handByPips[i].pips]++;
        suitCount[handByPips[i].suit]++;
    }
    if (hasFlush(suitCount)) {//同個花色大於五張
        int suit = getFlushSuit(suitCount);
        //filterHandBySuit(suit, handBySuit);//把不是同花色的牌拿掉
        filterHandBySuit(suit, handByPips);//把不是同花色的牌拿掉
        //if (hasRoyalFlush(handBySuit)) {//檢查是不是黃家同花順
        if (hasRoyalFlush(handByPips)) {//檢查是不是黃家同花順
            counts[0] += 1;
        }
            //else if (hasStraight(handBySuit)) {
        else if (hasStraight(handByPips)) {

            counts[1] += 1; //同花順+1
        }
        else {
            counts[4] += 1;//同花+1
        }

    }
    else if (!hasFlush(suitCount)) {
        int matches[3]; //統計 4條 3張 2張
        countCardPips(handByPips, HAND_SIZE, matches);

        if (hasStraight(handByPips)) {//順子
            counts[5] += 1;
        }
        else if (matches[0] != 0) {//4條
            counts[2] += 1;
        }
        else if (matches[1] != 0 && matches[2] != 0) { //3張 2張各有一對
            counts[3] += 1;
        }
        else if (matches[1] != 0 && matches[2] == 0) { //3張 一樣
            counts[6] += 1;
        }
            //else if (matches[1] == 0 && matches[2] == 2) { //2對
        else if (matches[1] == 0 && matches[2] >= 2) { //2對
            counts[7] += 1;
        }
        else if (matches[1] == 0 && matches[2] == 1) { //1對
            counts[8] += 1;
        }
        else {//其他
            counts[9] += 1;
        }


    }
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

// 排序並儲存結果函數
// 排序並儲存手牌
void sortAndStoreHand(Card hand[], Card handByPips[]) {
    // 複製手牌到 handByPips
    for (int i = 0; i < HAND_SIZE; i++) {
        handByPips[i] = hand[i];
    }

    // 按點數排序
    qsort(handByPips, HAND_SIZE, sizeof(Card), compareByPips);
}

int main() {
    srand(time(NULL));

    Card deck[NUM_CARDS];
    Card hands[NUM_PLAYERS][HOLE_CARDS];
    Card community[COMMUNITY_CARDS];
    Card combinedHand[HAND_SIZE];
    Card handByPips[HAND_SIZE];
    Card handBySuit[HAND_SIZE];
    int counts[10] = { 0 }; // 0: 皇家同花順, 1: 同花順, 2: 四條, 3: 葫蘆, 4: 同花, 5: 順子, 6: 三條, 7: 兩對, 8: 一對, 9: 高牌

    initializeDeck(deck);

    // 進行 NUM_HANDS 次迭代，每次迭代模擬一次完整的牌局
    for (int i = 0; i < NUM_HANDS; ++i) {
        // 將牌組進行洗牌
        shuffleDeck(deck, NUM_CARDS);

        // 將洗好的牌組發給玩家並發公共牌
        dealHands(deck, hands, community);

        // 對每個玩家的手牌進行處理
        for (int j = 0; j < NUM_PLAYERS; ++j) {
            // 將玩家的底牌和公共牌組合成最終的手牌
            combineHand(combinedHand, hands[j], community);

            // 排序並儲存手牌
            sortAndStoreHand(combinedHand, handByPips);

            // 評估手牌
            evaluateHand(handByPips, counts);
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