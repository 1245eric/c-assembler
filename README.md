這是一個使用 C 語言實作的簡單組譯器，能夠將自定義的 `.asm` 彙編語言程式轉換為二進位機器碼（`.bin`）。
每條指令佔用 2 bytes。


| 指令   | 說明               | 格式         |
|--------|-------------------|--------------|
| LOAD n | 載入數值 n         | `01 nn`      |
| ADD n  | 將累加器加上 n     | `02 nn`      |
| SUB n  | 將累加器減去 n     | `03 nn`      |
| STORE n| 儲存到記憶體位址 n | `04 nn`      |
| JMP L  | 無條件跳轉到標籤 L | `05 xx`      |
| JZ L   | 當結果為 0 跳轉    | `06 xx`      |
| HALT   | 停止執行           | `FF 00` 





ex: input,asm 


LOAD 0
STORE 100
LOAD 1
STORE 101
LOAD 10
STORE 102

LOOP:
LOAD 100
ADD 101
STORE 100

LOAD 101
ADD 1
STORE 101

LOAD 101
SUB 102
JZ END
JMP LOOP

END:
LOAD 100
HALT


expect output 


01 00  04 64  01 01  04 65  01 0A  04 66
01 64  02 65  04 64  01 65  02 01  04 65
01 65  03 66  06 20  05 0C  01 64  FF 00
