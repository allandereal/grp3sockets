char* ArithmeticOperators[5] = {"+", "-", "*", "/", "^"};

struct ArithmeticStructure {
    int firstNumber;
    int lastNumber;
    char operator;
};

int isExpression(char *word) {
    for (int i = 0; i < strlen(word); i++) {
        if ((word[i] == '+') || (word[i] == '-') || (word[i] == '/') || (word[i] == '*') || (word[i] == '^')) {
            return 1;
        }
    }
    return 0;
}

char* getArithmeticOperator(char *expression){
    for (int i = 0; i < strlen(expression); i++) {
        for(int j = 0; j < 5; j++){
            if(ArithmeticOperators[j][0] == expression[i]){
                return ArithmeticOperators[j];
            }
        }
    }
    return 0;
}

struct ArithmeticStructure createArithmeticStructure(char *expression){
    struct ArithmeticStructure aStructure ;
    char* operator;

    if((operator = getArithmeticOperator(expression)) != 0){
        aStructure.firstNumber = atoi(strtok(expression, operator));
        aStructure.lastNumber = atoi(strtok(NULL, operator));
        aStructure.operator = operator[0];
    }
    return aStructure;
}

int add(int firstNumber, int lastNumber){
    return firstNumber + lastNumber;
}

int subtract(int firstNumber, int lastNumber){
    return firstNumber - lastNumber;
}

int multiply(int firstNumber, int lastNumber){
    return firstNumber * lastNumber;
}

int divide(int firstNumber, int lastNumber){
    if(lastNumber == 0) return 0;
    return firstNumber / lastNumber;
}

int exponent(int number, int power){
    return pow(number, power);
}

int evaluateExpression(char *expression) {
    int result;
    struct ArithmeticStructure aStructure;
    aStructure = createArithmeticStructure(expression);
    
    switch(aStructure.operator){
        case '+':
            result = add(aStructure.firstNumber, aStructure.lastNumber);
            break;
        case '-':
            result = subtract(aStructure.firstNumber, aStructure.lastNumber);
            break;       
        case '*':
            result = multiply(aStructure.firstNumber, aStructure.lastNumber);
            break;
        case '/':
            result = divide(aStructure.firstNumber, aStructure.lastNumber);
            break;
        case '^':
            result = exponent(aStructure.firstNumber, aStructure.lastNumber);
            break;    
        default:
            result = 0;           
    }

    return result;
}