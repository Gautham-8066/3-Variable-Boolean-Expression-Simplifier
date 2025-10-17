/*
  3-Variable Boolean Expression Simplifier using Karnaugh Map Logic

  This Arduino project takes minterms (0–7) as input through the Serial Monitor.
  It then performs the following actions:
  1. Prints the complete truth table for the given minterms.
  2. Constructs the default Sum-of-Products (SOP) expression.
  3. Applies K-map simplification rules to output the minimized Boolean function.

  How to Use:
  1. Upload this code to your Arduino board.
  2. Open the Serial Monitor (Ctrl+Shift+M).
  3. Set the baud rate to 9600.
  4. Enter the minterms you want to simplify, separated by spaces (e.g., "1 3 4 5 7").
  5. Press Enter to see the results.
*/

// Global variable to store the bitmask of the 8 possible minterms (F(A,B,C)).
// Each bit corresponds to a minterm. For example, bit 0 is m0, bit 5 is m5.
byte minterm_mask = 0x00;

// Utility function to get the literal for a variable.
// If val is 1, it returns the variable (e.g., 'A').
// If val is 0, it returns the negated variable (e.g., "A'").
String lit(char var, int val) {
  return val ? String(var) : String(var) + "'";
}

// Prints the truth table based on the current minterm_mask.
void printTruthTable() {
  Serial.println("\n----------- TRUTH TABLE -----------");
  Serial.println(" A B C | F");
  Serial.println("---------------------------------");
  for (byte i = 0; i < 8; i++) {
    // Extract the values of A, B, and C from the index 'i'
    byte A = (i >> 2) & 1; // MSB
    byte B = (i >> 1) & 1;
    byte C = i & 1;        // LSB

    Serial.print(" ");
    Serial.print(A);
    Serial.print(" ");
    Serial.print(B);
    Serial.print(" ");
    Serial.print(C);
    Serial.print(" | ");
    // Check if the bit for the current minterm 'i' is set in the mask
    Serial.println((minterm_mask & (1 << i)) ? "1" : "0");
  }
  Serial.println("---------------------------------");
}

// Converts a minterm index (0-7) to its full literal expression (e.g., 5 -> AB'C).
String mintermToLiteral(int i) {
  return lit('A', (i >> 2) & 1) + lit('B', (i >> 1) & 1) + lit('C', i & 1);
}

// Builds the default Sum-of-Products (SOP) expression from the minterm mask.
// This is the unsimplified expression.
String buildDefaultSOP() {
  String sop = "";
  if (minterm_mask == 0) return "0";

  for (int i = 0; i < 8; i++) {
    if (minterm_mask & (1 << i)) {
      sop += mintermToLiteral(i) + " + ";
    }
  }
  // Remove the trailing " + "
  if (sop.endsWith(" + ")) {
    sop.remove(sop.length() - 3);
  }
  return sop;
}

// Simplifies the 3-variable function using Karnaugh Map (K-map) logic.
String simplifyFunction() {
  // Handle edge cases: all 0s or all 1s
  if (minterm_mask == 0x00) return "0";
  if (minterm_mask == 0xFF) return "1";

  // Array to keep track of which minterms have been covered by a group
  bool covered[8] = {false};
  String result = "";

  // -- Step 1: Check for Quads (groups of 4) --
  // A quad simplifies to a single literal.
  int quads[6][4] = {
    {0, 1, 4, 5}, // B'
    {2, 3, 6, 7}, // B
    {0, 2, 4, 6}, // C'
    {1, 3, 5, 7}, // C
    {0, 1, 2, 3}, // A'
    {4, 5, 6, 7}  // A
  };
  String quad_terms[] = {"B'", "B", "C'", "C", "A'", "A"};

  for (int i = 0; i < 6; i++) {
    int* q = quads[i];
    // Check if all minterms in the quad are present
    if ((minterm_mask & (1 << q[0])) && (minterm_mask & (1 << q[1])) &&
        (minterm_mask & (1 << q[2])) && (minterm_mask & (1 << q[3]))) {
      result += quad_terms[i] + " + ";
      // Mark these minterms as covered
      for (int j = 0; j < 4; j++) covered[q[j]] = true;
    }
  }

  // -- Step 2: Check for Pairs (groups of 2) --
  // A pair simplifies to a two-literal term.
  int pairs[12][2] = {
    {0, 1}, {2, 3}, {4, 5}, {6, 7}, // Horizontal pairs
    {0, 2}, {1, 3}, {4, 6}, {5, 7}, // Horizontal pairs (wrap-around)
    {0, 4}, {1, 5}, {2, 6}, {3, 7}  // Vertical pairs
  };
  String pair_terms[] = {"A'B'", "A'B", "AB'", "AB", "A'C'", "A'C", "AC'", "AC", "B'C'", "B'C", "BC'", "BC"};
  
  for (int i = 0; i < 12; i++) {
    int* p = pairs[i];
    // Check if both minterms in the pair are present and not already covered
    if ((minterm_mask & (1 << p[0])) && (minterm_mask & (1 << p[1])) &&
        (!covered[p[0]] || !covered[p[1]])) {
      result += pair_terms[i] + " + ";
      covered[p[0]] = covered[p[1]] = true;
    }
  }

  // -- Step 3: Check for Singletons (isolated 1s) --
  // These are minterms that could not be grouped.
  for (int i = 0; i < 8; i++) {
    if ((minterm_mask & (1 << i)) && !covered[i]) {
      result += mintermToLiteral(i) + " + ";
      // No need to mark as covered, as this is the last step.
    }
  }

  // Remove the trailing " + " from the final result
  if (result.endsWith(" + ")) {
    result.remove(result.length() - 3);
  }

  return result;
}

void setup() {
  Serial.begin(9600);
  Serial.println("\n--- 3-Variable Boolean Simplifier (A,B,C) ---");
  Serial.println("Enter minterms (0-7) separated by spaces, then press ENTER.");
  Serial.println("Example: 1 3 4 5 7");
}

void loop() {
  // Check if there is data available to read from the serial port
  if (Serial.available()) {
    minterm_mask = 0; // Reset the mask for new input
    String input = Serial.readStringUntil('\n');
    input.trim(); // Remove any leading/trailing whitespace

    int fromIndex = 0;
    // Parse the input string to set the bits in the minterm_mask
    while (fromIndex < input.length()) {
      int spaceIndex = input.indexOf(' ', fromIndex);
      if (spaceIndex == -1) {
        spaceIndex = input.length();
      }
      // Extract the number string and convert it to an integer
      String numStr = input.substring(fromIndex, spaceIndex);
      if (numStr.length() > 0) {
        int val = numStr.toInt();
        // Set the corresponding bit if the minterm is valid (0-7)
        if (val >= 0 && val < 8) {
          minterm_mask |= (1 << val);
        }
      }
      fromIndex = spaceIndex + 1;
    }

    // Print all the results
    printTruthTable();
    Serial.println("\n----------- EXPRESSIONS -----------");
    Serial.print("Default SOP: F = ");
    Serial.println(buildDefaultSOP());
    Serial.print("Simplified:  F = ");
    Serial.println(simplifyFunction());
    Serial.println("\n---------------------------------");
    Serial.println("Enter new minterms:");
  }
}

