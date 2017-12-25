namespace litexx {
    typedef boost::optional<std::string> ostring;
    typedef boost::optional<int> oint;
    typedef boost::optional<double> odouble;

    // LOAD COLUMN
    template<typename T>
    void loadColumn(sqlite3_stmt *stmt, int iCol, boost::optional<T>& c);

    template<class T>
    void bind(T const & entity, sqlite3_stmt *stmt);

    template<class T>
    void load(T & entity, sqlite3_stmt *stmt);

    template<class T>
    std::vector<T> loadAll(sqlite3_stmt *stmt);


    template<>
    void loadColumn(sqlite3_stmt *stmt, int iCol, oint& c){
        if(sqlite3_column_type(stmt, iCol) == SQLITE_NULL){
            c = boost::none;
        }
        else{
            c = oint(sqlite3_column_int(stmt, iCol));
        }
    }

    template<>
    void loadColumn(sqlite3_stmt *stmt, int iCol, odouble& c){
        if(sqlite3_column_type(stmt, iCol) == SQLITE_NULL){
            c = boost::none;
        }
        else{
            c = odouble(sqlite3_column_double(stmt, iCol));
        }
    }

    template<>
    void loadColumn(sqlite3_stmt *stmt, int iCol, ostring& c){
        if(sqlite3_column_type(stmt, iCol) == SQLITE_NULL){
            c = boost::none;
        }
        else{
            c = ostring((char *)sqlite3_column_text(stmt, iCol));
        }
    };

    template<typename T>
    inline void loadColumn(sqlite3_stmt *stmt, int iCol, boost::optional<T>& c){
        loadColumn<T>(stmt, iCol, c);
    }


    template<unsigned int _Idx>
    void _loadColumnsImpl(sqlite3_stmt *stmt){}

    template<unsigned int _Idx = 0, typename T, typename ...Args>
    void _loadColumnsImpl(sqlite3_stmt *stmt, boost::optional<T> & c, Args &... args){
        loadColumn(stmt, _Idx, c);
        _loadColumnsImpl<_Idx+1>(stmt, args...);
    }

    template<typename... Args>
    void loadColumns(sqlite3_stmt *stmt, Args &... args){
        _loadColumnsImpl(stmt, args...);
    }

    // BIND COLUMN
    template<typename T>
    void bindParam(sqlite3_stmt *stmt, int iCol, boost::optional<T> const & c);

    template<>
    void bindParam(sqlite3_stmt *stmt, int iCol, oint const & c){
        if(c){
            sqlite3_bind_int(stmt, iCol, *c);
        }
        else{
            sqlite3_bind_null(stmt, iCol);
        }
    }

    template<>
    void bindParam(sqlite3_stmt *stmt, int iCol, odouble const & c){
        if(c){
            sqlite3_bind_double(stmt, iCol, *c);
        }
        else{
            sqlite3_bind_null(stmt, iCol);
        }
    }

    template<>
    void bindParam(sqlite3_stmt *stmt, int iCol, ostring const & c){
        if(c){
            sqlite3_bind_text(stmt, iCol, c.get().c_str(), -1, SQLITE_TRANSIENT);
        }
        else{
            sqlite3_bind_null(stmt, iCol);
        }
    }

    template<unsigned int _Idx>
    void _bindParamsImpl(sqlite3_stmt *stmt){
    }

    template<unsigned int _Idx, typename T, typename ...Args>
    void _bindParamsImpl(sqlite3_stmt *stmt, boost::optional<T> const & c, Args const & ... args){
        bindParam(stmt, _Idx, c);
        _bindParamsImpl<_Idx+1>(stmt, args...);
    }

    template<typename... Args>
    void bindParams(sqlite3_stmt *stmt, Args const & ... args){
        _bindParamsImpl<1>(stmt, args...);
    }

    template<class T>
    void load(T & entity, sqlite3_stmt *stmt);

    template<class T>
    std::vector<T> loadAll(sqlite3_stmt *stmt){
        int rc;
        rc = sqlite3_step(stmt);

        std::vector<T> result;
        while(rc == SQLITE_ROW){
            T t;
            load<T>(t, stmt);
            result.push_back(t);

            rc = sqlite3_step(stmt);
        }
        sqlite3_finalize(stmt);

        return result;
    }

}

