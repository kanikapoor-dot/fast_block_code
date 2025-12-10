\# Fast Block Copy



Fast Block Copy is a small Windows utility that performs \*\*raw block-level copying\*\* from a  

Volume Shadow Copy (VSS snapshot) to a physical disk.  

It restores an entire snapshot directly onto a new or empty target disk, recreating the

partition table, filesystem, and files exactly as they existed at the time of the snapshot.



The tool simply reads from a shadow copy volume and writes its raw contents to an

unallocated or clean disk.



---



\## Step 1 — Create a VSS Snapshot



Use PowerShell to create a snapshot:



```cmd

powershell.exe -Command (gwmi -list win32\_shadowcopy).Create('E:\\','ClientAccessible')



\## Step 2 — List Shadow Copies



Use PowerShell



```cmd
vssadmin list shadows



Example Output:

Contents of shadow copy set ID: {bddca929-3521-4835-a73d-9a652d457c17}

&nbsp;  Contained 1 shadow copies at creation time: 09-12-2025 12:21:53

&nbsp;     Shadow Copy ID: {3ff6b662-1987-452c-896e-e174bfdb37d4}

&nbsp;        Original Volume: (E:)\\\\?\\Volume{9baa0984-4874-46ba-be28-ed74c43864b6}\\

&nbsp;        Shadow Copy Volume: \\\\?\\GLOBALROOT\\Device\\HarddiskVolumeShadowCopy9   <-- Source path

&nbsp;        Originating Machine: VOID-0





You will use the Shadow Copy Volume path as your source (-svp).



\## Step 3 — Prepare the Target Disk



You need an unallocated disk with no partitions.



Examples:



* Create a VHD
* Use a separate physical disk
* Use Disk Management to initialize the disk but do not create volumes



Determine the disk number using:



```cmd
diskpart

list disk



Example: Disk 2, Disk 3, etc.



\## Step 4 — Restore the Snapshot



Run Fast Block Copy:



```cmd
fast\_block\_copy.exe -svp "\\\\?\\GLOBALROOT\\Device\\HarddiskVolumeShadowCopy9" -tpp "\\\\.\\PhysicalDrive2"



Where:

* "\\\\?\\GLOBALROOT\\Device\\HarddiskVolumeShadowCopy9" → Source shadow copy
* "\\\\.\\PhysicalDrive2" → Target disk (change 2 based on your actual disk number)



\## Step 5 — Verify the Restore



After the tool finishes, it prints:



```cmd

Copy Completed





Windows will then:

* Read the restored partition table
* Mount the recreated NTFS volume
* Assign a new drive letter (e.g., G:)
* Show all restored files exactly as they existed in the snapshot



\## Repository Structure



fast\_block\_copy.cpp      – Main source file

README.md                – Documentation

.gitignore               – Visual Studio ignore rules

bin/                     – Prebuilt binaries



