# Typing Speed Checker Pro - Architecture Report

## 1. Executive Summary

The Typing Speed Checker Pro is a comprehensive C++ console application designed to measure and improve typing proficiency. It provides multiple difficulty levels, performance tracking, leaderboard functionality, and detailed analytics for users to monitor their typing progress over time.

---

## 2. System Architecture Overview

### 2.1 High-Level Architecture

The application follows an **Object-Oriented Design (OOD)** pattern with a single main class (`TypingSpeedChecker`) that encapsulates all functionality. The architecture is modular, with clear separation of concerns between data management, user interaction, and performance calculation.

```
┌─────────────────────────────────────────────────────┐
│                   Main Function                      │
│  - User authentication                               │
│  - Menu-driven interface                             │
└──────────────────┬──────────────────────────────────┘
                   │
                   ▼
┌─────────────────────────────────────────────────────┐
│            TypingSpeedChecker Class                  │
│  ┌───────────────────────────────────────────────┐  │
│  │  Data Management Layer                        │  │
│  │  - history (vector<TypingStats>)              │  │
│  │  - leaderboard (vector<TypingStats>)          │  │
│  │  - Text repositories (4 difficulty levels)    │  │
│  └───────────────────────────────────────────────┘  │
│  ┌───────────────────────────────────────────────┐  │
│  │  Core Business Logic                          │  │
│  │  - WPM calculation                            │  │
│  │  - Accuracy computation                       │  │
│  │  - Error detection                            │  │
│  └───────────────────────────────────────────────┘  │
│  ┌───────────────────────────────────────────────┐  │
│  │  User Interface Layer                         │  │
│  │  - Menu display                               │  │
│  │  - Results visualization                      │  │
│  │  - Performance analysis                       │  │
│  └───────────────────────────────────────────────┘  │
│  ┌───────────────────────────────────────────────┐  │
│  │  Persistence Layer                            │  │
│  │  - File I/O operations                        │  │
│  │  - Data serialization/deserialization         │  │
│  └───────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────┘
```

---

## 3. Data Architecture

### 3.1 Core Data Structure

**TypingStats Structure**
```cpp
struct TypingStats {
    string name;          // User identifier
    double wpm;           // Words per minute
    double accuracy;      // Percentage accuracy
    double time;          // Time taken in seconds
    int correctChars;     // Number of correct characters
    int totalChars;       // Total characters typed
    int errors;           // Number of errors made
    string difficulty;    // Test difficulty level
    string date;          // Timestamp of test
};
```

### 3.2 Data Storage

The application maintains three primary data collections:

1. **History Vector** (`vector<TypingStats> history`)
   - Stores complete test history for current user
   - Persisted to `typing_history.txt`
   - Supports unlimited historical records

2. **Leaderboard Vector** (`vector<TypingStats> leaderboard`)
   - Maintains top 10 performances across all users
   - Sorted by WPM (primary) and accuracy (secondary)
   - Persisted to `leaderboard.txt`

3. **Text Repositories** (4 vectors)
   - `easyTexts`: Simple sentences (3 entries)
   - `mediumTexts`: Moderate complexity (3 entries)
   - `hardTexts`: Technical content (3 entries)
   - `expertTexts`: Advanced technical material (3 entries)

### 3.3 File Persistence Format

**typing_history.txt format:**
```
name|wpm|accuracy|time|correctChars|totalChars|errors|difficulty|date
```

**leaderboard.txt format:**
```
name|wpm|accuracy|difficulty|date
```

---

## 4. Component Architecture

### 4.1 Core Components

#### 4.1.1 Performance Calculator
- **calculateWPM()**: Computes words per minute with validation
  - Formula: `(correctChars / 5) / (timeInSeconds / 60)`
  - Validation: Minimum 0.5s, maximum 300 WPM
  - Prevents timing anomalies on macOS

- **calculateAccuracy()**: Determines typing precision
  - Character-by-character comparison
  - Handles length mismatches
  - Returns percentage value

- **countErrors()**: Identifies typing mistakes
  - Counts mismatches, additions, and omissions
  - Uses maximum length for comparison

#### 4.1.2 Test Modes

1. **Standard Mode** (Easy/Medium/Hard/Expert)
   - Random text selection from difficulty pool
   - Untimed performance measurement
   - Comprehensive result analysis

2. **Timed Mode** (60 seconds)
   - Fixed 60-second duration
   - Repeating text for continuous typing
   - Real-time character tracking

3. **Custom Text Mode**
   - User-provided practice text
   - Multi-line input support
   - Flexible length testing

#### 4.1.3 Analysis & Visualization

- **displayResults()**: Basic statistics presentation
- **displayPerformanceAnalysis()**: Star ratings and feedback
- **displayRealTimeHighlight()**: Color-coded character matching
  - Green: Correct characters
  - Red: Incorrect characters
  - Yellow: Missing characters
- **displayDetailedComparison()**: Character-by-character breakdown

#### 4.1.4 Data Management

- **History Management**
  - `viewHistory()`: Display all past tests with statistics
  - `saveHistory()`: Serialize to file
  - `loadHistory()`: Deserialize from file

- **Leaderboard Management**
  - `updateLeaderboard()`: Add and sort top performers
  - `displayLeaderboard()`: Show top 10 with medals
  - Automatic pruning to maintain size limit

---

## 5. Algorithms & Logic Flow

### 5.1 Main Program Flow

```
START
  │
  ├─► Initialize TypingSpeedChecker
  │   ├─► Load history from file
  │   └─► Load leaderboard from file
  │
  ├─► Get username
  │   └─► Set current user
  │
  ├─► Display menu loop
  │   │
  │   ├─► Choice 1-4: Standard Test
  │   │   ├─► Select text by difficulty
  │   │   ├─► Display text & wait for ready
  │   │   ├─► Start timer
  │   │   ├─► Capture user input
  │   │   ├─► Stop timer
  │   │   ├─► Calculate metrics
  │   │   ├─► Save to history & leaderboard
  │   │   └─► Display results & analysis
  │   │
  │   ├─► Choice 5: Timed Mode
  │   │   └─► [Similar flow with 60s limit]
  │   │
  │   ├─► Choice 6: Custom Text
  │   │   └─► [Similar flow with user text]
  │   │
  │   ├─► Choice 7: View History
  │   │   └─► Display all records + averages
  │   │
  │   ├─► Choice 8: View Leaderboard
  │   │   └─► Display top 10 performers
  │   │
  │   └─► Choice 9: Exit
  │       ├─► Save history to file
  │       ├─► Save leaderboard to file
  │       └─► Exit program
  │
END
```

### 5.2 WPM Calculation Algorithm

```
FUNCTION calculateWPM(correctChars, timeInSeconds):
  IF timeInSeconds < 0.5 THEN
    RETURN 0  // Invalid timing
  END IF
  
  minutes = timeInSeconds / 60.0
  words = correctChars / 5.0  // Standard: 5 chars = 1 word
  wpm = words / minutes
  
  IF wpm > 300 THEN
    RETURN 0  // Unrealistic speed
  END IF
  
  RETURN wpm
END FUNCTION
```

### 5.3 Leaderboard Update Algorithm

```
FUNCTION updateLeaderboard(newStats):
  IF newStats.wpm <= 0 THEN
    RETURN  // Skip invalid entries
  END IF
  
  leaderboard.add(newStats)
  
  SORT leaderboard BY:
    PRIMARY: wpm (descending)
    SECONDARY: accuracy (descending)
  
  IF leaderboard.size > 10 THEN
    leaderboard.resize(10)  // Keep top 10 only
  END IF
END FUNCTION
```

---

## 6. User Interface Architecture

### 6.1 Menu System

The application uses a **numbered menu-driven interface** with emoji indicators:
- Options 1-4: Difficulty-based tests
- Option 5: Timed challenge
- Option 6: Custom practice
- Options 7-8: Data viewing
- Option 9: Exit

### 6.2 Color Coding System

Terminal color codes (ANSI escape sequences):
- `\033[32m`: Green (correct characters)
- `\033[31m`: Red (incorrect characters)
- `\033[33m`: Yellow (missing characters)
- `\033[0m`: Reset formatting

### 6.3 Visual Feedback Elements

1. **Progress Indicators**: Emoji-based status symbols
2. **Performance Ratings**: Star-based (⭐) rating system
3. **Medals**: Trophy system for leaderboard (🥇🥈🥉)
4. **Separators**: ASCII art borders for sections

---

## 7. Performance & Validation

### 7.1 Timing Mechanism

Uses `<chrono>` library for high-precision timing:
```cpp
auto startTime = chrono::high_resolution_clock::now();
// User types...
auto endTime = chrono::high_resolution_clock::now();
chrono::duration<double> elapsed = endTime - startTime;
```

### 7.2 Data Validation

1. **Timing Validation**
   - Minimum 0.5 seconds to prevent errors
   - Maximum 300 WPM to flag anomalies

2. **Input Validation**
   - Empty username defaults to "Guest"
   - Invalid menu choices trigger retry

3. **File Operations**
   - Graceful handling of missing files
   - Error checking on file open operations

---

## 8. Technical Specifications

### 8.1 Dependencies

```cpp
#include <iostream>     // Console I/O
#include <string>       // String operations
#include <vector>       // Dynamic arrays
#include <ctime>        // Date/time functions
#include <algorithm>    // Sorting algorithms
#include <sstream>      // String streaming
#include <iomanip>      // Output formatting
#include <fstream>      // File I/O
#include <chrono>       // High-precision timing
#include <thread>       // Threading support
```

### 8.2 Memory Management

- **Dynamic Memory**: Uses STL containers (automatic management)
- **Stack Allocation**: All objects created on stack
- **No Manual Deallocation**: RAII pattern ensures cleanup

### 8.3 Performance Characteristics

- **Time Complexity**
  - Text selection: O(1)
  - Accuracy calculation: O(n) where n = text length
  - Leaderboard sort: O(k log k) where k ≤ 10
  - History display: O(h) where h = history size

- **Space Complexity**
  - O(h + 10 + 4t) where:
    - h = history size
    - 10 = leaderboard limit
    - t = average text length

---

## 9. Extensibility & Future Enhancements

### 9.1 Current Limitations

1. Single-user session (no concurrent users)
2. Console-based interface only
3. Limited to English text
4. No network/cloud synchronization

### 9.2 Potential Enhancements

1. **GUI Implementation**: Qt or wxWidgets interface
2. **Multiplayer Mode**: Real-time typing races
3. **Progress Tracking**: Graph-based visualization
4. **Difficulty Adaptation**: AI-adjusted challenge levels
5. **Language Support**: Multi-language typing tests
6. **Statistics Export**: CSV/JSON export functionality
7. **Cloud Sync**: Cross-device progress tracking

---

## 10. Security & Data Integrity

### 10.1 Data Protection

- Local file storage (no network exposure)
- Plain-text storage (suitable for non-sensitive data)
- No password/authentication required

### 10.2 Data Integrity Measures

- Delimiter-based parsing (pipe-separated values)
- Error detection in WPM calculation
- Leaderboard size limiting prevents file bloat

---

## 11. Conclusion

The Typing Speed Checker Pro demonstrates a well-structured, modular architecture that balances functionality with simplicity. Its object-oriented design allows for easy maintenance and extension, while the comprehensive feature set provides users with valuable typing practice and performance insights. The application successfully combines real-time performance measurement, historical tracking, and competitive elements through its leaderboard system.

### Key Strengths:
- Robust performance calculations with validation
- Comprehensive user feedback and analysis
- Persistent data storage
- Multiple practice modes
- Clean, maintainable code structure

### Architecture Highlights:
- Single-class design with clear method responsibilities
- Efficient data structures (vectors for dynamic storage)
- File-based persistence for simplicity
- Terminal-based UI with visual enhancements

---

## Appendix: Code Metrics

- **Total Lines of Code**: ~650
- **Number of Methods**: 20+
- **Data Structures**: 1 struct, 1 class
- **File Dependencies**: 2 (history.txt, leaderboard.txt)
- **Difficulty Levels**: 4 standard + 2 special modes# typing-speed-cheaker-CLI
