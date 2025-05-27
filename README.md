# spade

```
WAYS FOR CONDITIONAL JUMPING IN BYTECODE

First type : (if statement)
+--------------------------------------------+
|    $cond:                                  |
|        ...                                 |
|        jxx $body   # conditional jump      |
|    $else:                                  |
|        ...                                 |    
|        jmp $merge                          |
|    $body:                                  |
|        ...                                 |
|    $merge:                                 |
|        ...                                 |
|        vret                                |
+--------------------------------------------+
Second type : (if statement)
+--------------------------------------------+
|    $cond:                                  |
|        ...                                 |
|        jxx $else   # conditional jump      |
|    $body:                                  |
|        ...                                 |    
|        jmp $merge                          |
|    $else:                                  |
|        ...                                 |
|    $merge:                                 |
|        ...                                 |
|        vret                                |
+--------------------------------------------+
Third type : (while statement)
+--------------------------------------------+
|    $cond:                                  |
|       ...                                  |
|       jxx $body                            |
|    $else:                                  |
|       ...                                  |
|       $end                                 |
|    $body:                                  |
|       ...                                  |
|       jmp $cond                            |
|    $end:                                   |
|        ...                                 |
|        vret                                |
+--------------------------------------------+
Fourth type : (while statement)
+--------------------------------------------+
|    $cond:                                  |
|       ...                                  |
|       jxx $else                            |
|    $body:                                  |
|       ...                                  |
|       jmp $cond                            |
|    $else:                                  |
|       ...                                  |
|    $end:                                   |
|        ...                                 |
|        vret                                |
+--------------------------------------------+
```