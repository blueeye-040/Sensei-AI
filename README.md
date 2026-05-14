# Project Invincible

To build a "perfect" system that is investor-ready, you need an architecture that balances low-level OS control (for invisibility) with high-level AI modularity (for intelligence).

Here is the comprehensive design, the high-level flow, and the folder structure for Project Invincible.

---

# 1. High-Level System Architecture

The system is designed as a **Multithreaded C++ Core**. This avoids the "heavy" overhead of web-based frameworks like Electron, making the app much harder to detect and lightning-fast.

## The Data Flow

```text
Ingestion:
AudioSentinel captures two streams (Mic + System Output).

↓
Transcription:
StreamProcessor feeds audio into a local Whisper.cpp instance.

↓
Reasoning:
IntelligenceCore sends the transcript + ContextStore (resume/notes) to a high-speed LLM.

↓
Display:
GhostUI renders the response using a Direct2D overlay with the exclusion flag active.
```

---

# 2. Professional Folder Structure

This structure follows industry standards for C++ projects, ensuring it can scale from a prototype to a full-scale startup product.

```text
InvincibleAssistant/
├── assets/                  # Icons, fonts (non-captured), and static UI resources
├── build/                   # Compiled .exe and build artifacts
├── docs/                    # Technical specs and investor pitch decks
├── include/                 # Header files (.h)
│   ├── AudioSentinel.h      # WASAPI capture logic
│   ├── GhostUI.h            # Win32 & Direct2D overlay logic
│   ├── IntelligenceCore.h   # API client & Prompt Engineering
│   ├── StreamProcessor.h    # Audio-to-Text orchestrator
│   └── Database.h           # SQLite/Vector storage headers
├── lib/                     # External libraries (Whisper.cpp, SQLite, nlohmann/json)
├── src/                     # Implementation files (.cpp)
│   ├── main.cpp             # Entry point & Global Event Loop
│   ├── AudioSentinel.cpp    # Real-time loopback implementation
│   ├── GhostUI.cpp          # OS-level invisibility & rendering
│   ├── IntelligenceCore.cpp # LLM integration (Gemini/Groq)
│   ├── StreamProcessor.cpp  # Buffer management for STT
│   └── Database.cpp         # Local context management
├── scripts/                 # Cross-compilation scripts (for Mac-to-Windows)
│   └── build_windows.sh     # MinGW build command
└── config.json              # API keys and user preferences (Excluded from Git)
```

---

# 3. Deep Dive: Component Design

---

## A. GhostUI (The Invincible State)

### Why

To ensure 100% invisibility, this component doesn't just use a flag; it manages "Display Affinity."

### Key Tech

```cpp
SetWindowDisplayAffinity(hwnd, WDA_EXCLUDEFROMCAPTURE);
```

### Low-Level Detail

You should implement Direct2D here. Unlike standard GDI windows, Direct2D allows you to render text with sub-pixel antialiasing that looks like a high-end HUD (Heads-Up Display).

---

## B. AudioSentinel (The Ear)

### Why

Capturing audio from a meeting without being "in" the meeting is tricky.

### Key Tech

**WASAPI Loopback**

It listens to the "Output" buffer of the Windows sound card.

### Logic

It separates your voice (Mic) from their voice (Speaker). This allows the AI to know who is asking the question and what you have already said.

---

## C. StreamProcessor (The Translator)

### Why

Sending raw audio to the cloud is slow and expensive.

### Key Tech

**Whisper.cpp (Local)**

### Logic

It uses a "Sliding Window" algorithm. It waits for a moment of silence (VAD - Voice Activity Detection) before finalizing a sentence and sending it to the Intelligence Core.

---

## D. IntelligenceCore (The Brain)

### Why

Raw transcripts are messy. This layer cleans them up.

### Key Tech

**Gemini 1.5 Flash**

### Logic

It uses "System Prompting" to act as your co-founder. It constantly compares the transcript against the ContextStore (your resume, project code, or startup data) to provide highly specific answers.

---

## E. ContextStore (The Memory)

### Why

The AI needs to know your specific "startup" or "tech" details.

### Key Tech

**SQLite + Simple Vector Search**

### Logic

When the user starts the app, they "drop" their PDF resume or text files into the app. This component indexes them so the AI can quote your own experience during the meeting.