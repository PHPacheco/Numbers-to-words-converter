# Transcribe Values in Full (English)

This project converts numeric values (up to one trillion) into their full text representation in **English**, supporting cents and formatting options.  
It features a **Windows GUI** built with **C++** and uses **C** for the core logic.

## Features

- Converts integer and decimal numbers to full text (e.g., `123.45` → `one hundred and twenty-three dollars and forty-five cents`)
- Supports values up to 1 trillion
- Validates user input with decimal and thousand separators
- Optionally adds parentheses around the output
- Optionally copies the result directly to the clipboard
- Copies the result to the clipboard when double-clicked
- User-friendly Windows desktop interface

![Example demonstration](https://github.com/PHPacheco/Numbers-to-words-converter/blob/main/Transcrever.png?raw=true)

## Technologies

- **C** — Number processing and conversion logic
- **C++** — Windows GUI (using Windows API)
- **Code::Blocks** project file (`.cbp`) included
- **Windows API** functions (clipboard interaction, window management)

## How to Use

1. Open the project file `Transcribe.cbp` with **Code::Blocks**.
2. Compile and run the program.
3. Enter a number in the input field (use `.` for thousand separator and `,` for decimals).
4. Press the button to transcribe the number into full English text.
5. (Optional) Copy the output to the clipboard or add parentheses.

## Input Rules

- Use the period `.` as a thousand separator (optional).
- Use the comma `,` as a decimal separator.
- Only numbers, periods, and commas are allowed.
- Example of valid inputs:
  - `1,000.00`
  - `123456,78`
  - `0,99`

## How to use

1. Open the `Transcribe.cbp` project in Code::Blocks.
2. Compile and run.
3. Enter a number and see its full text in the interface.

## License

[MIT License](LICENSE)

## Author

Pedro H. Pacheco
