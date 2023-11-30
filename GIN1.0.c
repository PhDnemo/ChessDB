PGDLLEXPORT Datum Chessgame_extract_value(PG_FUNCTION_ARGS);
//索引键
Datum
Chessgame_extract_value(PG_FUNCTION_ARGS)
{
    // 获取输入数据
    text *chessgameData = PG_GETARG_TEXT_P(0);
    
    // 用于存储提取的 FEN 字符串
    Datum *fenStrings;
    int nFenStrings = 0; // 用于记录 FEN 字符串的数量

    // 解析 chessgameData 以提取 FEN 字符串
    // 假设 chessgameData 是以某种方式存储多个 FEN 字符串
    // 这里需要一个函数来将 chessgameData 解析成多个 FEN 字符串
    fenStrings = ExtractFenStrings(chessgameData, &nFenStrings);

    // 将提取的 FEN 字符串数组作为结果返回
    PG_RETURN_POINTER(fenStrings);
}


Datum *
ExtractFenStrings(text *chessgameData, int *nFenStrings)
{
    // 实现解析逻辑
    // 这可能涉及到字符串分割、格式验证等，解析 chessgameData 并提取 FEN 字符串。
    // 返回一个 Datum 数组，其中每个元素是一个 FEN 字符串
}



PGDLLEXPORT Datum Chessgame_extract_query(PG_FUNCTION_ARGS);
//查询键
Datum
Chessgame_extract_query(PG_FUNCTION_ARGS)
{
    // 获取查询参数
    text *chessboardState = PG_GETARG_TEXT_P(0); // 特定的棋盘状态（FEN 字符串）
    int32 N = PG_GETARG_INT32(1); // 步数限制

    // 分配内存来存储提取的索引键
    Datum *queryKeys = (Datum *) palloc(sizeof(Datum));
    int32 *nkeys = (int32 *) PG_GETARG_POINTER(2);

    // 将查询的棋盘状态作为索引键
    queryKeys[0] = PointerGetDatum(chessboardState);
    *nkeys = 1; // 在这个例子中，我们只提取一个索引键

    // 可以选择性地存储额外信息（如步数限制）以供后续使用
    // ...

    // 返回提取的索引键
    PG_RETURN_POINTER(queryKeys);
}


PGDLLEXPORT Datum Chessgame_consistent(PG_FUNCTION_ARGS);
//比较二者
Datum
Chessgame_consistent(PG_FUNCTION_ARGS)
{
    // 获取从 extractQuery 提取的索引键和额外信息
    bool *check = (bool *) PG_GETARG_POINTER(0);
    int32 nkeys = PG_GETARG_INT32(3);
    Datum *queryKeys = (Datum *) PG_GETARG_POINTER(4);

    // 假设额外信息包含步数限制 "N"
    int32 N = ...; // 获取步数限制 N

    // 对于每个索引键，检查它是否与查询条件匹配
    for (int i = 0; i < nkeys; i++)
    {
        if (check[i])
        {
            text *chessboardState = DatumGetTextP(queryKeys[i]);
            
            // 检查棋盘状态是否与查询条件匹配
            // 需要比较 chessboardState 与索引项中的棋盘状态
            // 并考虑步数限制 N
            if (IsMatch(chessboardState, N))
            {
                PG_RETURN_BOOL(true);
            }
        }
    }

    // 如果没有匹配项，返回 false
    PG_RETURN_BOOL(false);
}

bool IsMatch(text *chessboardState, int32 N)
{
    // 实现用于比较棋盘状态的逻辑
    // 这可能涉及到解析 FEN 字符串，比较棋盘布局，以及考虑步数限制
}
