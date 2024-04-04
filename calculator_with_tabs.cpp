#include <Windows.h>
#include <commctrl.h>
#include <string>
#include <vector>

using namespace std;

//handlers and button positioning
const int EDIT_ID = 100;
const int BUTTON_ID = 200;
const int TRACE_BUTTON_ID = 250;
const int OP_ID = 300;
//const int DEBUG_ID = 400;     // used for optional debugging box present on Calculator tab.
const int TRACE_ID = 500;
const int CALC_ID = 600;

const int BUTTON_WIDTH = 60;
const int BUTTON_HEIGHT = 60;
const int GAP = 10;
const int BUTTON_X = 15;
const int BUTTON_Y = 170;

// toggle trace logging
bool TRACE_ENABLED = FALSE;

// hold button input for calculator
//string calcInput = "This is calcInput!";
// Will hold the entire entered number or latest operator.
// This is reset to "" when Clear Entry button is pressed.
string calcInput = "";

// Used for holding the expression to enter into the calculator function.
// Don't just append to this every time a button is pressed, but instead append the val of calcInput.
// That will allow the Clear Entry button to clear the latest number, but not the entire expression.
string expression = "";

// Allows debug logging
string debugInfo = "Debug Logging";

// trace information
string traceInfo = "Trace disabled.";
int decisionCounter = 0;



HWND createButton(int id, HWND parent_window, int x, int y, int width, string text);
void updateField(HWND parent_window, int handler, string text);
void updateTrace(HWND parent_window, string log_message);
string calculateInput(string input, HWND window);


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    static TCHAR szAppName[] = TEXT("windows");
    HWND hwnd;
    MSG msg;
    WNDCLASS wndclass;
    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;
    if (!RegisterClass(&wndclass))
    {
        MessageBox(NULL, TEXT("This program requires Windows NT!"), szAppName, MB_ICONERROR);
    }
    hwnd = CreateWindow(szAppName,
        TEXT("Five Function Calculator"),
        WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, // xor thickframe disables window resizing.
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        //CW_USEDEFAULT,
        (3 * BUTTON_X) + 5 * (BUTTON_WIDTH + GAP) + (3 * GAP),
        500,
        //CW_USEDEFAULT,
        NULL,
        NULL,
        hInstance,
        NULL);

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    while (GetMessageW(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HINSTANCE hInstance;

    // tab windows
    static HWND hwndTab = 0, calcWnd = 0, traceWnd = 0;
    //text fields
    static HWND textBox = 0, traceBox = 0;
    // number buttons
    static HWND button0 = 0, button1 = 0, button2 = 0, button3 = 0, button4 = 0, button5 = 0, button6 = 0, button7 = 0, button8 = 0, button9 = 0, buttonDec = 0;
    // operator buttons
    static HWND buttonC = 0, buttonCE = 0, buttonPlus = 0, buttonMinus = 0, buttonMult = 0, buttonDiv = 0, buttonPerc = 0, buttonEq = 0;
    // trace buttons
    static HWND traceOn = 0, traceOff = 0;

    TCITEM tie;
    RECT rcClient;
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_TAB_CLASSES;
    TCHAR tabLBL1[256];
    GetClientRect(hwnd, &rcClient);
    switch (message)
    {
    case WM_CREATE:
    {
        decisionCounter++;
        updateTrace(traceWnd, "Main loop case WM_CREATE entered.");

        hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
        hwndTab = CreateWindow(WC_TABCONTROL, "",
            WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
            0, 0, rcClient.right, rcClient.bottom,
            hwnd, NULL, hInstance, NULL);

        // Add tabs for calculator and trace logic
        tie.mask = TCIF_TEXT | TCIF_IMAGE;
        tie.iImage = -1;
        wsprintf(tabLBL1, "Calculator");
        tie.pszText = tabLBL1;
        TabCtrl_InsertItem(hwndTab, 0, &tie);
        wsprintf(tabLBL1, "Trace Logic");
        TabCtrl_InsertItem(hwndTab, 1, &tie);
        SendMessage(hwndTab, WM_SETFONT,
            reinterpret_cast<WPARAM>(GetStockObject(DEFAULT_GUI_FONT)), 0);

        calcWnd = CreateWindow(WC_STATIC, "",
            WS_CHILD | WS_VISIBLE,
            0, 20, 485, 480,        // Position and dimensions; example only.
            hwnd, (HMENU)CALC_ID, hInstance,    // hInstance is the global instance handle
            NULL);

        traceWnd = CreateWindow(WC_STATIC, "",
            WS_CHILD | WS_VISIBLE,
            0, 20, 485, 480,        // Position and dimensions
            hwnd, NULL, hInstance,    // hInstance is the global instance handle
            NULL);

        

         // Number buttons.  Ordered by position, not value.
        button7 = createButton(BUTTON_ID + 7, hwnd, BUTTON_X, BUTTON_Y, BUTTON_WIDTH, "7");
        button8 = createButton(BUTTON_ID + 8, hwnd, BUTTON_X + (BUTTON_WIDTH + GAP), BUTTON_Y, BUTTON_WIDTH, "8");
        button9 = createButton(BUTTON_ID + 9, hwnd, BUTTON_X + 2 * (BUTTON_WIDTH + GAP), BUTTON_Y, BUTTON_WIDTH, "9");
        button4 = createButton(BUTTON_ID + 4, hwnd, BUTTON_X, BUTTON_Y + (BUTTON_HEIGHT + GAP), BUTTON_WIDTH, "4");
        button5 = createButton(BUTTON_ID + 5, hwnd, BUTTON_X + (BUTTON_WIDTH + GAP), BUTTON_Y + (BUTTON_HEIGHT + GAP), BUTTON_WIDTH, "5");
        button6 = createButton(BUTTON_ID + 6, hwnd, BUTTON_X + 2 * (BUTTON_WIDTH + GAP), BUTTON_Y + (BUTTON_HEIGHT + GAP), BUTTON_WIDTH, "6");
        button1 = createButton(BUTTON_ID + 1, hwnd, BUTTON_X, BUTTON_Y + 2 * (BUTTON_HEIGHT + GAP), BUTTON_WIDTH, "1");
        button2 = createButton(BUTTON_ID + 2, hwnd, BUTTON_X + (BUTTON_WIDTH + GAP), BUTTON_Y + 2 * (BUTTON_HEIGHT + GAP), BUTTON_WIDTH, "2");
        button3 = createButton(BUTTON_ID + 3, hwnd, BUTTON_X + 2 * (BUTTON_WIDTH + GAP), BUTTON_Y + 2 * (BUTTON_HEIGHT + GAP), BUTTON_WIDTH, "3");
        button0 = createButton(BUTTON_ID + 0, hwnd, BUTTON_X, BUTTON_Y + 3 * (BUTTON_HEIGHT + GAP), (2 * BUTTON_WIDTH + GAP), "0");
        buttonDec = createButton(BUTTON_ID + 10, hwnd, BUTTON_X + 2 * (BUTTON_WIDTH + GAP), BUTTON_Y + 3 * (BUTTON_HEIGHT + GAP), BUTTON_WIDTH, ".");

        // Operator buttons
        buttonC = createButton(OP_ID + 0, hwnd, BUTTON_X + 3 * (BUTTON_WIDTH + GAP) + (3 * GAP), BUTTON_Y, BUTTON_WIDTH, "C");
        buttonCE = createButton(OP_ID + 1, hwnd, BUTTON_X + 4 * (BUTTON_WIDTH + GAP) + (3 * GAP), BUTTON_Y, BUTTON_WIDTH, "CE");
        buttonPlus = createButton(OP_ID + 2, hwnd, BUTTON_X + 3 * (BUTTON_WIDTH + GAP) + (3 * GAP), BUTTON_Y + (BUTTON_HEIGHT + GAP), BUTTON_WIDTH, "+");
        buttonMinus = createButton(OP_ID + 3, hwnd, BUTTON_X + 4 * (BUTTON_WIDTH + GAP) + (3 * GAP), BUTTON_Y + (BUTTON_HEIGHT + GAP), BUTTON_WIDTH, "-");
        buttonMult = createButton(OP_ID + 4, hwnd, BUTTON_X + 3 * (BUTTON_WIDTH + GAP) + (3 * GAP), BUTTON_Y + 2 * (BUTTON_HEIGHT + GAP), BUTTON_WIDTH, "*");
        buttonDiv = createButton(OP_ID + 5, hwnd, BUTTON_X + 4 * (BUTTON_WIDTH + GAP) + (3 * GAP), BUTTON_Y + 2 * (BUTTON_HEIGHT + GAP), BUTTON_WIDTH, "/");
        buttonPerc = createButton(OP_ID + 6, hwnd, BUTTON_X + 4 * (BUTTON_WIDTH + GAP) + (3 * GAP), BUTTON_Y + 3 * (BUTTON_WIDTH + GAP), BUTTON_WIDTH, "%");
        buttonEq = createButton(OP_ID + 7, hwnd, BUTTON_X + 3 * (BUTTON_WIDTH + GAP) + (3 * GAP), BUTTON_Y + 3 * (BUTTON_HEIGHT + GAP), BUTTON_WIDTH, "=");

        traceOff = CreateWindow(
            "BUTTON",  // Predefined class; Unicode assumed 
            "Intro Trace Off",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
            BUTTON_X + 5 * (BUTTON_WIDTH + GAP) + (3 * GAP) - (BUTTON_WIDTH * 1.75),         // x position 
            30,         // y position
            BUTTON_WIDTH * 1.75,        // Button width
            30,        // Button height
            hwnd,     // Parent window
            (HMENU)(TRACE_BUTTON_ID),       // Button handler.
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);      // Pointer not needed.

        traceOn = CreateWindow(
            "BUTTON",  // Predefined class; Unicode assumed 
            "Intro Trace On",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
            BUTTON_X + 5 * (BUTTON_WIDTH + GAP) + (2 * GAP) - (2 * (BUTTON_WIDTH * 1.75)),         // x position 
            30,         // y position
            BUTTON_WIDTH * 1.75,        // Button width
            30,        // Button height
            hwnd,     // Parent window
            (HMENU)(TRACE_BUTTON_ID + 1),       // button handler
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);      // Pointer not needed.


        // create text boxes
        textBox = CreateWindow(
            "EDIT", calcInput.c_str(), WS_BORDER | WS_CHILD | WS_VISIBLE,
            BUTTON_X, 60, 5 * (BUTTON_WIDTH + GAP) + (2 * GAP), 30,
            calcWnd, (HMENU)EDIT_ID, 0, 0);

      //  debugBox = CreateWindow(
      //      "EDIT", debugInfo.c_str(), WS_BORDER | WS_CHILD | WS_VISIBLE,
      //      BUTTON_X, 400, 5 * (BUTTON_WIDTH + GAP) + (2 * GAP), 30,
       //     calcWnd, (HMENU)DEBUG_ID, 0, 0);

        traceBox = CreateWindow(
            "EDIT", traceInfo.c_str(), WS_BORDER | WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY | WS_VSCROLL,
            5, 10, BUTTON_X + 5 * (BUTTON_WIDTH + GAP) + (3 * GAP), 400,
            traceWnd, (HMENU)TRACE_ID, 0, 0);


        ShowWindow(calcWnd, TRUE);
        ShowWindow(traceWnd, FALSE);
        
        break;
    }

    case WM_COMMAND:
        decisionCounter++;
        debugInfo = "Message ID: " + to_string(LOWORD(wParam));
        updateTrace(traceWnd, "Main loop case WM_COMMAND entered (a button was pressed!)");

        decisionCounter++;
        updateTrace(traceWnd, "Check if a number button was pressed.");

         // if a number button is pressed (including .)
        if (BUTTON_ID <= LOWORD(wParam) && LOWORD(wParam) <= BUTTON_ID + 10)
        {

            decisionCounter++;
            updateTrace(traceWnd, "Check if previous input was an operator (+-*/%)");
            // if calcInput was an operator previously, append it to expression and reset calcInput
            if ((calcInput == "+") or (calcInput == "-") or (calcInput == "*") or (calcInput == "/") or (calcInput == "%"))
            {
                expression += calcInput;
                calcInput = "";
            }

            // get the number pressed
            const int digit = (LOWORD(wParam)) - BUTTON_ID;

            decisionCounter++;
            if (digit == 10)
            {
                debugInfo = "Digit button . was pressed.";
                calcInput += ".";
            }
            else
            {
                debugInfo = "Digit button " + to_string(digit) + " was pressed.";
                calcInput += to_string(digit);
            }
            updateTrace(traceWnd, "Check if . was pressed, else a number was pressed.");

            updateField(calcWnd, EDIT_ID, calcInput);
        }
        // If an operator button is pressed
        else if ((OP_ID <= LOWORD(wParam) and LOWORD(wParam) <= OP_ID + 7))
        {
            decisionCounter++;
            updateTrace(traceWnd, "An operator button (+-*/%, C, CE) was pressed.");

            decisionCounter++;
            updateTrace(traceWnd, "Check if C or CE button was pressed");
            // if C, reset calcInput and expression.  Set edit_field to "".
            // if CE, reset calcInput and set edit_field to "".
            if ((OP_ID == LOWORD(wParam)) or ((OP_ID + 1) == LOWORD(wParam)))
            {
                debugInfo = "CE button pressed.";

                decisionCounter++;
                updateTrace(traceWnd, "Check if C button was pressed.");
                if (OP_ID == LOWORD(wParam))
                {
                    expression = "";
                    debugInfo = "C button pressed.";
                }
                calcInput = "";

                updateField(calcWnd, EDIT_ID, calcInput);
            }


            // if operator, append calcInput to expression to add the latest number.  Set calcInput to operator.
            else if (((OP_ID + 2) <= LOWORD(wParam)) and ((OP_ID + 6) >= LOWORD(wParam)))
            {
                decisionCounter++;
                updateTrace(traceWnd, "If an operator (+-*/%) was pressed, need to append correct operator.");

                expression += calcInput;
                debugInfo = "Current value of expression: " + expression;
                int op = LOWORD(wParam) - OP_ID;

                // append the correct operator to the expression
                switch (op)
                {
                    // 2 is + sign
                case 2:
                {
                    decisionCounter++;

                    debugInfo = "+ sign was pressed.";
                    updateTrace(traceWnd, "Append + sign.");

                    calcInput = "+";
                    break;
                }
                case 3:
                {
                    decisionCounter++;
                    debugInfo = "- sign was pressed.";
                    updateTrace(traceWnd, "Append - sign.");

                    calcInput = "-";
                    break;
                }
                case 4:
                {
                    decisionCounter++;
                    debugInfo = "* sign was pressed.";
                    updateTrace(traceWnd, "Append * sign.");

                    calcInput = "*";
                    break;
                }
                case 5:
                {
                    decisionCounter++;
                    debugInfo = "/ sign was pressed.";
                    updateTrace(traceWnd, "Append / sign.");

                    calcInput = "/";
                    break;
                }
                case 6:
                {
                    decisionCounter++;
                    debugInfo = "% sign was pressed.";
                    updateTrace(traceWnd, "Append % sign.");

                    calcInput = "%";
                    break;
                }
                }

                updateField(calcWnd, EDIT_ID, calcInput);
            }

            // if =, need to calculate the string (call with try except).  Append the latest calcInput first to get the latest input.  Need to print out the sum returned.
            else if ((OP_ID + 7) == LOWORD(wParam))
            {
                decisionCounter++;
                debugInfo = "= sign was pressed.";
                expression += calcInput;
                expression += "=";
                updateTrace(traceWnd, "Check that = was pressed.  Begin to calculate expression.");

                // Calculate the expression given by user.
                // calculateInput has some built-in checks, but in case something strange happens the program will catch the error.
                try
                {
                    calcInput = calculateInput(expression, traceWnd);
                }
                catch (...)
                {
                    calcInput = "Invalid expression.";
                }

                expression = "";

                updateField(calcWnd, EDIT_ID, calcInput);
               // updateField(calcWnd, DEBUG_ID, debugInfo);
                //  updateField(traceWnd, TRACE_ID, debugInfo);
            }
        }
        // Checking if a trace button was pressed.
        else if ((LOWORD(wParam) >= TRACE_BUTTON_ID) and (LOWORD(wParam) <= TRACE_BUTTON_ID + 1))
        {
            decisionCounter++;
            int id = LOWORD(wParam) - TRACE_BUTTON_ID;

            debugInfo = "A trace button was pressed.  ID: " + to_string(id);
            updateTrace(traceWnd, "Check that a trace button was pressed.");



            decisionCounter++;
            updateTrace(traceWnd, "Check that trace off is pressed.");
            if (id == 0)
            {
                // trace off pressed
                TRACE_ENABLED = FALSE;
                debugInfo = "Intro Trace Off button pressed.";
                updateField(traceWnd, TRACE_ID, "Trace disabled.");
            }
            else
            {
                // trace on pressed
                debugInfo = "Intro Trace On button pressed.";

                decisionCounter++;
                updateTrace(traceWnd, "Check that trace is not enabled.");
                if (TRACE_ENABLED == FALSE)
                {
                    TRACE_ENABLED = TRUE;
                    traceInfo = "";
                    decisionCounter = 0;
                    decisionCounter++;
                    updateField(traceWnd, TRACE_ID, "");
                    debugInfo = "Intro Trace On button pressed.";

                    updateTrace(traceWnd, "Trace logging enabled.");
                }
                
            }
        }

        break;

    case WM_DESTROY:
        decisionCounter++;
        debugInfo = "Program window closed.";
        updateTrace(traceWnd, "Main loop case WM_DESTROY.");

        PostQuitMessage(0);
        return 0;

    case WM_NOTIFY:
        decisionCounter++;
        debugInfo = "Control handle: " + to_string(lParam);
        updateTrace(traceWnd, "Main loop case WM_NOTIFY entered.  Used for tab control.");


        decisionCounter++;
        updateTrace(traceWnd, "Check if a tab was changed.");
        if (((LPNMHDR)lParam)->code == TCN_SELCHANGE)
        {
            int tabID = TabCtrl_GetCurSel(hwndTab);
            bool visible = FALSE;

            decisionCounter++;
            debugInfo = "Tab ID: " + to_string(tabID);
            updateTrace(traceWnd, "Check which tab was selected and update accordingly.");
            if (tabID == 0)
            {
                visible = TRUE;
            }

            // !visible is used for showing tab 2.  visible is used for showing tab 1.
            ShowWindow(traceWnd, !visible);

            // contains text box
            ShowWindow(calcWnd, visible);

            //trace buttons
            ShowWindow(traceOff, visible);
            ShowWindow(traceOn, visible);

            // number buttons
            ShowWindow(button0, visible);
            ShowWindow(button1, visible);
            ShowWindow(button2, visible);
            ShowWindow(button3, visible);
            ShowWindow(button4, visible);
            ShowWindow(button5, visible);
            ShowWindow(button6, visible);
            ShowWindow(button7, visible);
            ShowWindow(button8, visible);
            ShowWindow(button9, visible);
            ShowWindow(buttonDec, visible);

            //operator buttons
            ShowWindow(buttonC, visible);
            ShowWindow(buttonCE, visible);
            ShowWindow(buttonPlus, visible);
            ShowWindow(buttonMinus, visible);
            ShowWindow(buttonMult, visible);
            ShowWindow(buttonDiv, visible);
            ShowWindow(buttonPerc, visible);
            ShowWindow(buttonEq, visible);
        }

        break;

    default:
        // The program lags when the default case is documented.  Therefore, for demonstration purposes, it has been removed from logging.

        //decisionCounter++;
       // debugInfo = "No message passed.";
       // updateTrace(traceWnd, "Main loop default case entered.");
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
    //return DefWindowProc(hwnd, message, wParam, lParam);
}



// Function for creating buttons.
HWND createButton(int id, HWND parent_window, int x, int y, int width, string text)
{
    return CreateWindow(
        "BUTTON",  // Predefined class; Unicode assumed 
        text.c_str(),      // Button text 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
        x,         // x position 
        y,         // y position
        width,        // Button width
        BUTTON_HEIGHT,        // Button height
        parent_window,     // Parent window
        (HMENU)(id),       // Button handler.
        (HINSTANCE)GetWindowLongPtr(parent_window, GWLP_HINSTANCE),
        NULL);      // Pointer not needed.
}

// Updates a single EDIT box.
void updateField(HWND parent_window, int handler, string text)
{
    const HWND field = GetDlgItem(parent_window, handler);
    SetWindowText(field, text.c_str());
}

// Takes in an expression as a string and the trace output window (for logging purposes), and returns the calcuted output.
string calculateInput(string input, HWND window)
{
    double sum = 0;
    string result = "";
    string currentNum = "";
    double newNum;
    string operators = "";
    vector<double> numbers;

    // the first step is separating numbers and operators into two different lists
    // to create the number, a string of the number will be put together.  Once the number is finished (next char is an operator) it is checked for validity, then cast to a double.


    // if first character is -, number, or decimal, append it to the first number.  Otherwise, return invalid.

    for (int i = 0; i < input.length(); i++)
    {
        decisionCounter++;
        debugInfo = "i: " + to_string(i) + "  length: " + to_string(input.length());
        updateTrace(window, "Iterate over each character in the expression using for loop.");
        
        
        decisionCounter++;
        debugInfo = "Current char: ";
        debugInfo += input.at(i);
        updateTrace(window, "For every char, check if char is -, . or a number.");

        if ((input[i] == '-') || (input[i] == '.') || (isdigit(input.at(i))))
        {
            decisionCounter++;
            updateTrace(window, "Check if current char is a - that is not the first character.");
            if ((i != 0) && (input[i] == '-'))
            {

                decisionCounter++;
                debugInfo = "Prev char: ";
                debugInfo += input.at(i - 1);
                updateTrace(window, "Check if previous char was a number to confirm if current char is minus or negative sign.");
                 //because the previous character was a number or decimal point, that means this is a minus sign.  Otherwise, it's a negative sign
                if ((((isdigit(input.at(i - 1))))) || (input[i - 1] == '.'))
                {

                    operators += input[i];
                    newNum = stod(currentNum);
                    numbers.emplace_back(newNum);
                    currentNum = "";
                }
                else
                {
                    currentNum += input[i];
                }
            }
            else
            {
                currentNum += input[i];
            }
        }
        // we have an operator
        else
        {

            decisionCounter++;
            debugInfo = "Prev num: " + currentNum;
            updateTrace(window, "B/C current char is operator, check that the latest number is not empty.");
            if (currentNum != "")
            {
                newNum = stod(currentNum);
            }

            decisionCounter++;
            debugInfo = "Current char: ";
            debugInfo += input[i];
            updateTrace(window, "Check if current char is %");
            if (input[i] == '%')
            {

                decisionCounter++;
                debugInfo = "Size of numbers list: ";
                debugInfo += to_string(numbers.size());
                updateTrace(window, "Check to make sure % is not being appended to the first number.");
                // can't take percent if we're handling the first number, need a previous number
                if (numbers.size() == 0)
                {
                    result = "invalid input";
                    return result;
                }
                else
                {
                    // want the second to last element, and we start counting from element 0
                    int prevNum = numbers.size() - 1;
                    newNum = (newNum / 100) * numbers[prevNum];
                }

            }

            decisionCounter++;
            debugInfo = "Current number: " + currentNum;
            updateTrace(window, "Check to see if there is a number to append to the list of numbers.");
            if (currentNum != "")
            {
                numbers.emplace_back(newNum);
            }

            decisionCounter++;
            debugInfo = "Current char: ";
            debugInfo += input[i];
            updateTrace(window, "Check if the operator is %.  If not, then append to list of operators.");
            if (input[i] != '%')
            {
                operators += input[i];
            }

            currentNum = "";

        }
    }

       // now for part 2!  Adding up the final sum.
       // first needs to check that we have the same number of nums and operators (because operators includes =)

    decisionCounter++;
    debugInfo = "Size of numbers: ";
    debugInfo += to_string(numbers.size());
    debugInfo += "   Size of operators: ";
    debugInfo += to_string(operators.size());
    updateTrace(window, "Begin calculation.  Check to make sure there are the same number of numbers as operators (= is included).");
    if (operators.size() != numbers.size())
    {
        result = "invalid input!";
        return result;
    }


    int N = numbers.size();
    double num2;
    char op;

    sum = numbers[0];

    for (int i = 1; i < N; i++)
    {
        decisionCounter++;
        debugInfo = "i: " + to_string(i) + "  length: " + to_string(N);
        updateTrace(window, "Iterate over each number in list of numbers using for loop.");

        num2 = numbers[i];
        op = operators[i - 1];

        debugInfo = "Sum: " + to_string(sum) + "  operator: ";
        debugInfo += op;
        debugInfo += "  2nd num: " + to_string(num2);

        switch (op)
        {
        case '+':
        {
            decisionCounter++;
            updateTrace(window, "Operator is +.  Add to sum.");
            sum = (sum + num2);
            break;
        }
        case '-':
        {
            decisionCounter++;
            updateTrace(window, "Operator is -.  Subtract from sum.");
            sum = (sum - num2);
            break;
        }
        case '*':
        {
            decisionCounter++;
            updateTrace(window, "Operator is *.  Multiply with sum.");
            sum = (sum * num2);
            break;
        }
        case '/':
        {
            decisionCounter++;
            updateTrace(window, "Operator is /.  Divide from sum.");
            sum = (sum / num2);
            break;
        }
        default:
        {           
            break;
        }
        }
    }

    // convert the final result to a string, and return the answer.
    result = to_string(sum);
    return result;
}

// Updates the trace log window specifically using a predetermined format.
// NOTICE: A decision is made inside for the purpose of easily disabling trace output.  This decision is not recorded and documented in the trace output.
void updateTrace(HWND parent_window, string log_message)
{
    if (TRACE_ENABLED == TRUE)
    { 
        //traceInfo += "\r\nCheck if trace logging is enabled.\r\nDecision Count: ";
        //traceInfo += to_string(decisionCounter);
        //traceInfo += "\r\n";
        //updateField(parent_window, TRACE_ID, traceInfo);

        //decisionCounter++;  // increment because of the decision.  However, increment inside so the decision count is accurately recorded.

        traceInfo += "\r\nDecision: ";
        traceInfo += log_message;
        traceInfo += "\r\nDebug Info: ";
        traceInfo += debugInfo;
        traceInfo += "\r\nCurrent Expression: ";
        traceInfo += expression;
        traceInfo += "\r\nDecision Count: ";
        traceInfo += to_string(decisionCounter);
        traceInfo += "\r\n";
        updateField(parent_window, TRACE_ID, traceInfo);
    }
}
