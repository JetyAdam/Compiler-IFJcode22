# Překladač jazyka IFJ22

## Postup vývoje
|       Status        | Task                                              | Login                               |
|:-------------------:|---------------------------------------------------|-------------------------------------|
|       :gear:        | [MVP](#mvp)                                       | xletak00,xjetma02,xosine00,xslaby10 | 
| :heavy_check_mark:  | [Globální definice](#globální-definice)           | xletak00                            |
|       :gear:        | [Lexikální analyzátor](#lexikální-analyzátor)     | xslaby10, xjetma02                  |
|       :gear:        | [Syntaktycký analizátor](#syntaktycký-analizátor) | xosine00, xslaby10                  |
|       :gear:        | [Výrazy](#výrazy)                                 | xjetma02                            |
|       :gear:        | [Sémantický analyzátor](#sémantický-analyzátor)   | xjetma02                            |
|       :gear:        | [Generátor kódu](#generátor-kódu)                 | xletak00                            |
|       :gear:        | [Tabulka symbolů](#tabulka-symbolů)               | xosine00                            |
|       :gear:        | [Vestavěné funkce](#vestavěné-funkce)             | xletak00                            |

Status legend:

| Not started yet | Work in progress |        Done        |
|:---------------:|:----------------:|:------------------:|
|                 |      :gear:      | :heavy_check_mark: |

### Přibližné termíny dokončení
|          Modul          |  Termíny   |
|:-----------------------:|:----------:|
|    Globální definice    | 10.10.2022 |
|  Lexikální analyzátor   | 16.10.2022 |
| Syntaktický analyzátor  | 13.11.2022 |
|         Výrazy          | 30.10.2022 |
|  Sémantický analyzátor  | 13.11.2022 |
|     Generátor kódu      | 13.11.2022 |
|     Tabulka symbolů     | 23.10.2022 |
|    Vestavěné funkce     | 23.10.2022 |
|     Funkční projekt     | 20.11.2022 |
|           MVP           | 30.10.2022 |
|    Pokusné odevzdání    | 24.11.2022 |
|    Finální odevzdání    | 7.12.2022  |

### MVP
Minimální přeložitelné řešení projektu. Například program:
```php
<?php
    declare(strict_types=1);
    $test = "This is a test";
?>
```
Musí být přeložen na program v jazyku IFJcode22:
```
.IFJcode22
DEFVAR GF@var_00001
MOVE GF@var_00001 string@This\032is\032a\032test
```

### Globální definice
Soubor s užitečnými globálními definicemi jako např. `#define MC_ON` nebo 
`enum tokens{WS,IF,WHILE,...};` 

### Lexikální analyzátor
Lexikální analyzátor podle konečného automatu je realizován v souboru [lex_analyz.c](/lex_analyz.c)

### Syntaktycký analizátor
Syntaktický analyzátor podle LL-gramatiky je realizován v souboru [syn_analyz.c](/syn_analyz.c)    

V minulém projektu jsem vytvořil tento zápis LL-gramatiky kde neterminály (funkce) jsou zapsané v
`<>` a terminály (tokeny) jskou zapsané s prefixem `%` pro speciální tokeny je použito značení `%{}`
a uvnitř co značí npř `%{WS}` značí token white space, `%{EOF}` token end of file.
```
/**
 * Pravidla Syntaktické analýzy
 * <PROGRAM> -> %include %string <LINE-LIST>
 * <LINE-LIST> -> <LINE> %{WS} <LINE-LIST>
 * <LINE-LIST> -> %{EOF}
 * <LINE> -> %global <G-DEF>
 * <LINE> -> %function <F-DEF>
 * <LINE> -> %local <LOCAL> %{WS}
 * <LINE> -> <ASSIGNMENT> %{WS}
 * <LINE> -> %return <ASIGN-VALS> %{WS}
 * <LINE> -> %if <IF-DEF>
 * <LINE> -> %while <WHILE-DEF>
 * <G-DEF> -> %ID %: <G-TYPE>
 * <G-TYPE> -> %integer
 * <G-TYPE> -> %number
 * <G-TYPE> -> %string
 * <G-TYPE> -> %nil
 * <G-TYPE> -> %function %( <F-DEC-PAR-LIST> %: <F-DEC-RET-LIST>
 * <G-TYPE> -> %function %( <F-DEC-PAR-LIST>
 * <F-DEC-PAR-LIST> -> %)
 * <F-DEC-PAR-LIST> -> <ID-TYPE> %)
 * <F-DEC-PAR-LIST> -> <ID-TYPE> %, <F-DEC-PAR-LIST2>
 * <F-DEC-PAR-LIST2> -> <ID-TYPE> %)
 * <F-DEC-PAR-LIST2> -> <ID-TYPE> %, <F-DEC-PAR-LIST2>
 * <F-DEC-RET-LIST> -> <ID-TYPE> %{WS}
 * <F-DEC-RET-LIST> -> <ID-TYPE> %, <F-DEC-RET-LIST>
 * <WHILE-DEF> -> <E> %do <LINE-LIST> %end
 * <IF-DEF> -> <E> %then <LINE-LIST> %else <LINE-LIST> %end
 * <IF-DEF> -> <E> %then <LINE-LIST> %end
 * <F-DEF> -> %ID %( <F-PAR-LIST> %: <F-RET-LIST> <LINE-LIST> %end
 * <F-DEF> -> %ID %( <F-PAR-LIST> <LINE-LIST> %end
 * <ASSIGNMENT> -> %ID %( <F-CALL-PAR>
 * <ASSIGNMENT> -> %ID <ASSIGN-LIST> %= <ASIGN-VALS>
 * <ASSIGN-LIST> ->  %, <ASSIGN-LIST>
 * <ASSIGN-LIST> ->  %=
 * <ASIGN-VALS> -> <E> %, <ASIGN-VALS>
 * <ASIGN-VALS> -> <E> %{WS}
 * ?<ASIGN-VALS> -> %ID %( %)
 * ?<ASIGN-VALS> -> %ID %( <F-IN-LIST>
 * ?<F-IN-LIST> -> <E> %)
 * ?<F-IN-LIST> -> <E> %, <F-IN-LIST>
 * <LOCAL> -> %ID %: <ID-TYPE>
 * <LOCAL> -> %ID %: <ID-TYPE> %= <E>
 * <F-PAR-LIST> -> %ID %: <ID-TYPE> %, <F-PAR-LIST2>
 * <F-PAR-LIST> -> %ID %: <ID-TYPE> %)
 * <F-PAR-LIST> -> %)
 * <F-PAR-LIST2> -> %ID %: <ID-TYPE> %, <F-PAR-LIST2>
 * <F-PAR-LIST2> -> %ID %: <ID-TYPE> %)
 * <F-RET-LIST> -> <ID-TYPE> %, <F-RET-LIST>
 * <F-RET-LIST> -> <ID-TYPE> %{WS}
 * <F-CALL-PAR> -> %)
 * <F-CALL-PAR> -> <CALL_TYPE> %)
 * <F-CALL-PAR> -> <CALL_TYPE> %, <F-CALL-PAR2>
 * <F-CALL-PAR2> -> <CALL_TYPE> %)
 * <F-CALL-PAR2> -> <CALL_TYPE> %, <F-CALL-PAR2>
 * <CALL_TYPE> -> %ID
 * <CALL_TYPE> -> %_STRING
 * <CALL_TYPE> -> %_NUMBER
 * <CALL_TYPE> -> %_INT
 * <CALL_TYPE> -> %nil
 * <ID-TYPE> -> %integer
 * <ID-TYPE> -> %string
 * <ID-TYPE> -> %number
 * <ID-TYPE> -> %nil
 */
```


### Výrazy
Jsou realizovány v souboru [expr.c](/expr.c)

### Sémantický analyzátor
Je realizován v souboru [sem_analyz.c](/sem_analyz.c)

### Generátor kódu
Je realizován v souboru [code_gen.c](/code_gen.c)

### Tabulka symbolů
Je realizována v souboru [sym_table.c](/sym_table.c)

Tabulka symbolů bude implementována jako BVS.

### Vestavěné funkce
Je realizován v souboru [inbuild.c](/inbuild.c)
