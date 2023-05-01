class Matrix3 {
public:
    float data[9];

    Matrix3() {
        for (int i = 0; i < 9; ++i) {
            data[i] = 0;
        }
    }

    float get(int row, int col) const {
        return data[row * 3 + col];
    }

    void set(int row, int col, float value) {
        data[row * 3 + col] = value;
    }

    // Other matrix operations, such as matrix multiplication, etc.
};