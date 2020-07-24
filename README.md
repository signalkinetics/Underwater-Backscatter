# Underwater Backscatter

We present Piezo-Acoustic Backscatter (PAB), the first technology that enables backscatter networking in underwater environments.

Since wireless communication is the largest source of energy consumption for many underwater sensors, transitioning to
backscatter technology would eliminate the need for batteries which increase size and cost and require frequent replacement. Battery-free underwater sensors would enable us to **sense ocean conditions** (such as acidity, temperature, and bacteria content) over extended
periods of time and understand how they correlate with **climate change**. Scientists may attach these sensors to marine animals
and use them to **understand migration and habitat patterns**. Such sensors may even be **used in space missions to search for life** in the recently discovered subsurface oceans of Saturn’s moon, Titan. More generally, underwater battery-free sensors can be leveraged in
many long-term ocean applications such as **naval deployments, oil spill monitoring, and scientific exploration**

A PAB system consists of an acoustic projector (transmitter), a hydrophone (receiver), and a battery-free PAB sensor (which is essentially a piezoceramic transducer). When the projector transmits acoustic signals underwater, a PAB sensor harvests energy from these signals and powers up a microcontroller which controls the logic for backscatter. The PAB sensor then communicates by modulating the reflections of ambient sound signals. In particular, it can transmit a ‘0’ bit by absorbing the incoming energy, and a ‘1’ bit by reflecting the impinging acoustic signal. It can switch between the reflective and absorptive states by modulating the voltage across the piezoelectric interface, which in turn determines its vibration amplitude (i.e., reflection). The hydrophone receives the acoustic signals, senses changes in the amplitude due to reflection, and decodes these changes to recover the transmitted messages. In our system, we use PWM signals to trasmit data on the downlink and FM0 modulation  to send data on the uplink channel.

![](/Images/PAB.PNG)
**PAB System Design**

![](/Images/PAB_node.jpeg)
**PAB sensor node**

# Contents

**MATLAB Rx Files/** - MATLAB code files for decoding the recieved FM0 signal.

**MATLAB Tx Files/** - MATLAB code files for transmitting a PWM acoustic signal.

**MCU code/** - C++ code for microcontroller (which controls backscatter logic).

**PCB Eagle Schematic/** - Circuit design for PCB which is connected to the PAB node.

**SolidWorks 3D Models/** - 3D printing models for mold, transducer caps and washers.

**PAB Tutorials/** - Detailed tutorials for fabricating PAB nodes and setting up the PAB system.

# Getting Started
The best way of manufacturing the PAB system is to first build the system hardware and then to work on the software part. There are several files in the "PAB Tutorials" folder which will guide you to build the system. To manufacture a PAB node, you would need a piezocermaic cylinder (we recommend ordering them from Steminc) and then 3D printing the remaining parts for assembly (SolidWorks 3D Models/). You would also need to connect wires to the inner and outer surface of the piezoceramic cylinder which will be used later to connect the PAB node to a custom made PCB with a microcontroller (PCB Eagle Schematic/). You can use the MCU code files to implement the backscatter logic in the microcontroller (MCU code/). Finally, you will need a piezoceramic transmitter and a hydrophone to transmit and receive signals respectively. We used MATLAB code to send data on the downlink and to decode the backscattered messages on the uplink (MATLAB Tx Files/, MATLAB Rx Files/)


# PAB Hardware Design
To fabricate a PAB node, you will need to get a piezoceramic cylinder and you would also need polyurethane resin (we recommend using WC-575A/B from BJP enterprise) for casting purposes. Once you have soldered wires on both sides of the piezoceramic cylinder, you would need to enclose it using 3D printed caps (with laser-cut rubber washers and screw) and then place it inside a 3D printed mold for casting (you can open the 3D models by installing the latest version of SolidWorks software). Finally you can cure the entire structure by placing it inside a pressure chamber at 4psi for at least 9 hours (see PAB tutorials for more details).

You would also need to print the PCB and populate it with all the necessary circuit components. The PCB circuit layout along with cicuit component specifications can be opened using the open source Eagle software by Autodesk (PCB Eagle Schematic/). The PCB needs to be connected to the two terminals of the potted piezoceramic cylinder node to complete the fabrication process of a PAB backscatter sensor. As a final step, you can program the MCU on the PCB by compiling the C++ code "main.c" on the microcontroller (MCU code/).

**Congratulations!** You have succesfully built a fully working, battery-free PAB sensor!


# PAB Transciever
To communicate with the PAB sensor you would need to send PWM acoustic pulses using the MATLAB code in "MATLAB Tx Files/" . To achieve this, you can simply fabricate a fully-potted piezoceramic transducer (i.e. omit the caps, washers and screws before casting) and use it as a projector. Connect the two leads of this transducer to the output of an amplifier (we recommend using Xli2500 Two-channel 750W power amplifier) and send the PWM signals by connecting the input terminals of the amplifier to a laptop using a audio jack. To deocde data on the uplink, you can simply use Audacity software to record the received sound pulses using a H2A Hydrophone and later you can decode this data offline by running "Main_Rx.m" in "MATLAB Rx Files/"

We hope that you will enjoy fabricating and deploying this PAB system in new and exciting applications. 

**Good Luck and Happy Back-scattering! :smiley:**

# Acknowledgement

**Advisor/PI:** Fadel Adib.

**Authors:** Sayed Saad Afzal, Osvy Rodriguez, Reza Gaffarivardavagh.

**Collaborators:** JunSu Jang, Jose Muguira Iturralde.

**For in-depth information on how PAB works:** 

Paper: [**Underwater Backscatter Networking**](https://www.media.mit.edu/projects/oceans/overview/), JunSu Jang and Fadel Adib [*ACM SIGCOMM'19*](https://conferences.sigcomm.org/sigcomm/2019/), Beijing, China, August 2019.

Demo Video: [Taking our ocean's pulse: Underwater Backscattering Networking](https://www.youtube.com/watch?v=zC3HaY6YJLY)

Presentation Video: [SIGCOMM 2020 talk](https://dl.acm.org/ft_gateway.cfm?id=3342091&ftid=2080731&dwn=1&CFID=153853516&CFTOKEN=83cd046e67f484a4-561F895A-CC29-7729-70CF6744AE201ECC)

# Citation
```
@incollection{jang2019underwater,
  title={Underwater backscatter networking},
  author={Jang, Junsu and Adib, Fadel},
  booktitle={Proceedings of the ACM Special Interest Group on Data Communication},
  pages={187--199},
  year={2019}
}

@incollection{gaffarivardavagh2020ultra-wideband,
  title={Ultra-Wideband Underwater Backscatter via Piezoelectric Metamaterials},
  author={Gaffarivardavagh, Reza and Afzal, Sayed Saad and Rodriguez, Osvy and Adib, Fadel},
  booktitle={Proceedings of the ACM Special Interest Group on Data Communication},
  year={2020}
}

```

Shield: [![CC BY-NC-SA 4.0][cc-by-nc-sa-shield]][cc-by-nc-sa]

This work is licensed under a
[Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License][cc-by-nc-sa].

[![CC BY-NC-SA 4.0][cc-by-nc-sa-image]][cc-by-nc-sa]

[cc-by-nc-sa]: http://creativecommons.org/licenses/by-nc-sa/4.0/
[cc-by-nc-sa-image]: https://licensebuttons.net/l/by-nc-sa/4.0/88x31.png
[cc-by-nc-sa-shield]: https://img.shields.io/badge/License-CC%20BY--NC--SA%204.0-lightgrey.svg
