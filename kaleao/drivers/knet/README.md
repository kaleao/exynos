
KMAX 10G Exynos (KGEX) Ethernet Driver		{#kgex}
=====================================

KGEX is the 10g ethernet driver for the exynos components.

It is a custom 10G ethernet driver that (in conjunction with the (@ref pci_exynos)) manipulates
the custom FPGA hardware for providing network connectivity to the exynoses.

KGEX is a standard linux ethernet driver and follows the Linux pci network API guidelines. It also
is responsible for checking the state of the PCI link and takes appropriate action in case of errors.
It has a reviver mechanism (@ref kgexerrorhandlingreviver) to detect and correct errors that occur on the PCI link.



Hardware and Specifications			{#kgexhwspecs}
===========================

KGEX controls the main networking component of the exynos processor. The network interfaces are
located in the Network Processing Unit (NPU) and Storage Procesing Unit (SPU) FPGAs. The
connection between the 'firmware' and the exynos SOC is done via 2 PCI Root Complexes located
in the SOCs.

The hardware of the network interface consists of a single-channel DMA per FPGA per exynos, each
of which are connected on a switch that is also located in each FPGA.


PCI Link					{#kgexhwspecspcilink}
--------

Each exynos has 2 PCI root complexes, capable of delivering Gen2 (5 Mbps) pci traffic, each one
connected to the FPGAs. On the FPGA side lies a PCI Endpoint that acts as the network device
(providing memory access to the DMA register space). They use MSI-X interrupts for signaling
the processor. For each 'network card' there are 2 interrupt lines, one for RX and one for TX.

Apart from the DMA register space, there is also a 'shared memory' region on one of the FPGA
subsystems (the NPU). Using this memory the exynos chipset can communicate with the board
manager (the NPU FPGA processing system) for exchanging information and/or commands. For this
purpose KGEX exports certain /proc entries.

The PCI links have some stability issues which, along with other error cases, are addressed
using the reviver software (@ref kgexreviver).


AXI DMA						{#kgexhwspecsaxidma}
-------

The main networking device is the Xilinx AXI DMA. The driver utilizes this piece of hardware
as a single-channel per direction DMA for sending/receiving network packets. The network traffic is moved
using the Scatter-Gather mode of the DMA. So in total two seperate channels are used, one for RX
and one for TX, but we have 1 single queue per direction.

The DMA provides 2 methods of completion signalling, polling or interrupt. For the interrupt
mode, there is an interrupt pin for each direction, so 2 interrupt pins for the TX and RX channels.

In Scatter-Gather mode the DMA must be fed with two lists of descriptors (RX/TX) which the driver is
responsible of creating and managing.

For more information on the AXI DMA:
https://www.xilinx.com/support/documentation/ip_documentation/axi_dma/v7_1/pg021_axi_dma.pdf


Interrupts					{#kgexhwspecsinterrupts}
---------

As discussed above there are 2 seperate interrupt pins one for each direction. These interrupt
pins are fed into a custom hardware block that translates them into MSI-X interrupts that
then reach the PCI root complex.

The exynos SOC root complex does not provide more than one seperate physical interrupt lines.
This results in both interrupts being routed to the same core. As a result, traffic in the RX
channel can affect the performance of the TX channel and vice versa. This issue has been addressed
with the introduction of the NAPI and custom-hybrid TX operations (@ref kgexrxtxrxpath,
@ref kgexrxtxtxpath).


Network Receive/Transmit Mechanisms		{#kgexrxtx}
===================================

From the network point of view KGEX uses hybrid interrupt/polling mechanisms for serving both the
incoming and outgoing traffic.


RX path						{#kgexrxtxrxpath}
-------

On the receive path of the driver the linux kernel New API (NAPI) infrastructure is used.

This method of processing incoming traffic uses an interrupt to trigger a polling session of RX
packets. While in this polling session the interrupts disabled not as long as there are more packets
to be processed. This increases the throughput because less interrupts need to be generated and
served. It also decreases the latency for a single packet, since the interrupt coalescing value
used to achieve higher throughput is 1.

For more information about NAPI: https://wiki.linuxfoundation.org/networking/napi


TX path						{#kgexrxtxtxpath}
-------

On the transmit path we have implemented a NAPI-style (hybrid interrupt/polling) method for processing
sent packets and clearing the TX ring.

The completion interrupt is used to trigger a polling session of the TX ring for completed descriptors.
All the packets that have completed are passed to the higher stack layers. This method reduces the
interrupts being generated for completed packets and also gives us the freedom to do a 'virtual'
seperation of the RX and TX paths, even though the interrupt lines for the RX and TX paths go to
the same core (see @ref kgexhwspecsinterrupts).



KGEX Error Handling				{#kgexerrorhandling}
===================

In any network device a number of errors can occur. Especially if the data link is not very stable.
In our case, there are several errors that can occur, from the DMA to send wrong data or get stuck
to the PCI link going down while in the middle of a transaction. To address these issues a software
module is used to detect and correct those issues. There is also a hardware part that is used for
the whole network card to reset.


Error List					{#kgexerrorhandlingerrorlist}
----------

Here is a synopsis of the possible error cases that can occur:

|               Error case             | Component |              Impact            |
|:------------------------------------:|:---------:|:------------------------------:|
|               PCI gen1               | PCI RC-EP | The link needs to be retrained |
|         DMA transfer error           |    DMA    |  The DMA and link needs reset  |
|         PCI EP unreadable            | PCI RC-EP |  The DMA and link needs reset  |
| TX transfer is ongoing (TX is stuck) |    DMA    |  The DMA and link need reset   |



KGEX reviver mechanism 				{#kgexerrorhandlingreviver}
----------------------

The reviver mechanism is a piece of software incorporated in the network driver that detects and
corrects PCI link errors (link down, corrupted information in packets or interrupt lines, etc.).

It uses a timer that is configurable at runtime (with a default value of 100 jifies, ~0.3 seconds),
to check every possible error case. If the reviver finds that one of the above errors is present
it takes the appropriate course of action. For the time being, on all errors the course of action
is resetting both the PCI link and the DMA and its peripherals.

Here is flowchart of the reviver module:

\dot
  digraph example {
      node [shape=record, fontsize=25];
      a [ label="Start" ];
      b [ label="PCI gen2 ?" ];
      c [ label="DMA transfer error ?" ];
      d [ label="PCI EP unreadable (0xFFFFFFFF) ?" ];
      e [ label="Ongoing TX transfer ?" ];
      f [ label="Stop network activity" ];
      g [ label="Request external reset (link + DMA)"];
      h [ label="Start network activity"];
      i [ label="Reschedule check"];
      j [ label="End"];

      a -> b;
      b -> c [ label="Yes" ];
      b -> f [ label="No" ];
      c -> d [ label="No" ];
      c -> f [ label="Yes" ];
      d -> e [ label="No" ];
      d -> f [ label="Yes" ];
      e -> h [ label="No" ];
      e -> f [ label="Yes" ];
      f -> g;
      g -> h;
      h -> i;
      i -> j;

  }
\enddot



KGEX Reference Manual				{#kgexrefman}
=====================

Function reference is below.
