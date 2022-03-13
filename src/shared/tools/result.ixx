export module tools.result;

export template<class Ok, class Err>
class result {
private:
    bool state { };

public:
    Ok  ok  { };
    Err err { };

    result(Ok   ok_value) : state { true  }, ok  {  ok_value } { }
    result(Err err_value) : state { false }, err { err_value } { }

    operator bool() { return state; }
};

export template<class Ok>
class result<Ok, void> {
private:
    bool state { };

public:
    Ok ok { };

    result(Ok ok_value) : state { true  }, ok { ok_value } { }
    result()            : state { false }                  { }

    operator bool() { return state; }
};

export template<class Err>
class result<void, Err> {
private:
    bool state { };

public:
    Err err { };

    result()              : state { true  }                    { }
    result(Err err_value) : state { false }, err { err_value } { }

    operator bool() { return state; }
};
