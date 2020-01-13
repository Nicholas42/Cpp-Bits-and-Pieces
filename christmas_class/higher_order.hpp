template<class Ret, class LHS, class RHS>
struct lazy_evaluation {
    lazy_evaluation (Ret (*f) (LHS, RHS), const LHS& l, const RHS& r) : func(f), lhs(l), rhs(r)
    {}

    Ret operator()() {
        return func(lhs, rhs);
    }

private:
    Ret (*func) (LHS, RHS);
    LHS lhs;
    RHS rhs;
};

int add(int i, int j) {
    return i + j;
}

