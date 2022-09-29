# SIC-SICXE-Assembly
功能: 切好的token進行分析、檢查文法錯誤，實作含指定pseudo instruction的Assembler，在pass1時給予合法的指令對應的位址，再經由pass2將指令轉為對應的object code ，最後將每行指令所對應的資訊寫成output 檔。
data structure:
TokenPtr為切好的Token，存放每個Token的資訊(table, num)
Table1-4為指定Table，Table5放Symbol，Table6放Integer/Real，Table7放String，Table5-7皆以mod100的hash function放入範圍為0-99的array中。
Symbol Table(symtab)為放置Symbol及其對應位址的array，放置方法與Table5相同
Literal Table(literTab)為放置literal及其對應位址的array，放置方法與Table5-7相同
Literal Buffer(literBuf)為放置尚未定義之literal的vector
line為存放每一行指令資訊(line, location, object code, 是否錯誤, SICXE指令的format)的vector
