# MIDI Creator


This project generates a MIDI file that plays a simple melody. It is designed to run on a Raspberry Pi 4 with a 64-bit architecture.

# Features

- Generates a MIDI file named `output.mid` with a custom melody.
- Plays the generated MIDI file automatically using `TiMidity++`.

# Requirements

- Raspberry Pi 4 (64-bit OS)
- Cross-compiling environment on your host machine (e.g., Ubuntu)
- `g++` cross-compiler targeting `aarch64` (64-bit ARM)
- `timidity` installed on Raspberry Pi for MIDI playback

---

# Step 1: Install Cross-Compiler on Host Machine

On your host machine (e.g., Ubuntu), install the cross-compiler for ARM64:

```bash
sudo apt update
sudo apt install g++-aarch64-linux-gnu
```

---

# Step 2: Cross-Compile the Program

Use the following command to cross-compile `midi_creator.cpp` for Raspberry Pi 4:

```bash
git clone git@github.com:EmNetLab411/Multimedia-Embedded-Systems.git
cd Multimedia-Embedded-Systems/MIDICreator/
aarch64-linux-gnu-g++ midi_creator.cpp -o midi_creator -static
```

- `aarch64-linux-gnu-g++`: Specifies the cross-compiler for ARM64.
- `-o midi_creator`: Sets the output file name.
- `-static`: Statically links libraries to avoid dependency issues on the Raspberry Pi.

---

# Step 3: Transfer Executable to Raspberry Pi

Copy the compiled executable to your Raspberry Pi using `scp`:

```bash
scp midi_creator pi@<RPI_IP_ADDRESS>:/home/pi/
```

Replace `<RPI_IP_ADDRESS>` with the IP address of your Raspberry Pi.

---

# Step 4: Install TiMidity++ on Raspberry Pi

SSH into your Raspberry Pi and install `timidity` for MIDI playback:

```bash
ssh pi@<RPI_IP_ADDRESS>
sudo apt update
sudo apt install timidity
```

---

# Step 5: Run the Program on Raspberry Pi

Make the executable runnable and execute it:

```bash
chmod +x midi_creator
./midi_creator
```

This will generate `output.mid` and play it using `timidity`.

---

# Troubleshooting

- If you encounter permission errors, try running the program with `sudo`.
- Ensure `timidity` is correctly installed and configured on Raspberry Pi.
- If cross-compilation fails, check the installation of `g++-aarch64-linux-gnu`.

---
