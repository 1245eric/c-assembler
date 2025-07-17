#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 100
#define MAX_PROGRAM 1024

// 用來儲存標籤名稱與對應的位址
typedef struct {
    char label[20];
    int address;
} Label;

Label labels[100]; // 儲存所有標籤
int label_count = 0; // 紀錄標籤數量

// 根據標籤名稱尋找對應的位址
int find_label_address(const char *name) {
    for (int i = 0; i < label_count; i++) {
        if (strcmp(labels[i].label, name) == 0)
            return labels[i].address;
    }
    return -1;
}


// 將助記符轉換為對應的 opcode
int get_opcode(const char *mnemonic) {
    if (strcmp(mnemonic, "LOAD") == 0) return 0x01;
    if (strcmp(mnemonic, "ADD") == 0) return 0x02;
    if (strcmp(mnemonic, "SUB") == 0) return 0x03;
    if (strcmp(mnemonic, "STORE") == 0) return 0x04;
    if (strcmp(mnemonic, "JMP") == 0) return 0x05;
    if (strcmp(mnemonic, "JZ") == 0) return 0x06;
    if (strcmp(mnemonic, "HALT") == 0) return 0xFF;
    return -1;
}

// 去除每行後方的換行字元
void strip_newline(char *str) {
    str[strcspn(str, "\n")] = 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s input.asm output.bin\n", argv[0]);
        return 1;
    }

    FILE *fin = fopen(argv[1], "r");
    if (!fin) {
        perror("Input file error");
        return 1;
    }

    char program[MAX_PROGRAM][MAX_LINE]; //暫存原始程式
    int address = 0, line_count = 0;

    // 第一輪：找出標籤與其對應的位址
    while (fgets(program[line_count], MAX_LINE, fin)) {
        char *line = program[line_count];
        strip_newline(line);
        char label[20];

        // 偵測是否有標籤（例如 LOOP:）
        if (sscanf(line, "%[^:]:", label) == 1) {
            strcpy(labels[label_count].label, label);
            labels[label_count].address = address;
            label_count++;
        } else {
            address += 2; /*條指令佔 2 bytes*/
        }
        line_count++;
    }
    rewind(fin); //重置檔案指標

    FILE *fout = fopen(argv[2], "wb");
    if (!fout) {
        perror("Output file error");
        return 1;
    }

    //第二輪：實際轉換成機器碼
    for (int i = 0; i < line_count; i++) {
        char line[MAX_LINE];
        strcpy(line, program[i]);
        strip_newline(line);

        if (strchr(line, ':')) continue; //忽略標籤定義行

        char mnemonic[10], operand_str[20];
        int operand = 0;

        //分析指令與操作數
        if (sscanf(line, "%s %s", mnemonic, operand_str) >= 1) {
            int opcode = get_opcode(mnemonic);
            if (opcode == -1){
                continue; //忽略不認識的指令
            }
            //halt是不帶參數的指令
            if (opcode == 0xFF) {
                unsigned char halt[2] = { (unsigned char)opcode, 0x00 };
                fwrite(halt, 1, 2, fout);
                continue;
            }

            //檢查操作數是否是標籤
            if (isalpha(operand_str[0])) {
                operand = find_label_address(operand_str);
                if (operand == -1) operand = 0;//找不到標籤預設為 0
            } else {
                operand = atoi(operand_str);//數字直接轉換
            }

            //輸出 2-byte
            unsigned char bytecode[2];
            bytecode[0] = (unsigned char)opcode;
            bytecode[1] = (unsigned char)operand;
            fwrite(bytecode, 1, 2, fout);
        }
    }

    fclose(fin);
    fclose(fout);
    return 0;
}
