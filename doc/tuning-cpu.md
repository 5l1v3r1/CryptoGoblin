
 ************************
 * THREAD CONFIG BASICS *
 ************************

The first thing you need to do is run the miner with the config file unchanged, that will cause the miner
to print out a suggested thread config. This config might not always be optimal, but it is a good starting point.

Your CPU has a certain amount of L2 and/or L3 cache, you can find this with cpu-z or even better by looking
up your cpu model on cpu-world.com.

For all Intel cpus, it is the last-level cache that counts in this calculation. Only L3 if it has such, or else only L2 cache.

For older AMD cpus, you can add L2 + L3 cache to get the amount relevant to us.
For new AMD cpus (zen/ryzen-based or newer), only last-level cache counts, so only L3 cache.

Each normal miner thread needs 2MB of cache. So if your cpu has 8MB of cache, you can run four normal miner
threads, each eating 2MB cache. OR you can run two double-threads, since those need 4MB each.

Each miner thread should run on a separate physical cpu core for optimal speed.
If your cpu support hyper-threading, then finding the core numbers is a small challenge:

In Linux, first the primary hyperthreads per core are listed, then the secondaries.
So running 4 threads on a 4-core/8-thread cpu, you need to run threads with affinity on cores: 1,2,3,4

In Windows, the primary and secondary hyperthreads are interleaved.
So running 4 threads on a 4-core/8-thread cpu, you need to run threads with affinity on cores: 1,3,5,7

If your cpu has more cache than you can use with normal threads on all cores, you should run additional
normal threads on one or more secondary hyperthreads. If your cpu does not have hyperthreads, then you
will need to run some of the miner threads in double-mode.

After you have found your optimal config, try flipping ONE thread per cpu to the opposite of the rest,
this will pretty much make that thread the designated victim in a cache-starved situation. That thread
might now be slower, but the rest will be faster, so hopefully you gained a few H/s total.

If your cpu is overheating or you need to use the machine for other things as well and it gets sluggish,
then you can replace a pair of normal miner threads with a single miner thread in double-mode. This
only gets you 80-85% of the hashrate of two normal threads, but it will save power and will free up one
core for other uses. (But cache will still be full, you might want to free up cache if you want to 
use the computer while mining. YMMV)


 **********************
 * LARGE PAGE SUPPORT *
 **********************

Large pages (aka Huge pages) is very beneficial for miners, since the cpu TLB cache only needs to have a
a single entry per 2MB scratchpad. While with normal 4KB pages, it would need to hold 512 pages. The problem
with that is that for most cpus, this will exhaust the whole TLB cache just to hold the scratchpad, leaving
nothing for other parts of the miner program or data, or for other processes in the OS.
The performance difference between normal pages and large pages is around 20%, depending on system and cpu.

If you run your miner within a VM, then hugepages will often not make much, if any difference.

Large pages need a properly set up OS. It can be difficult if you are not used to systems administation,
but the performace results are worth the trouble. Slow memory mode is meant as a backup.


 ** Windows **
CryptoGoblin is able to set up Large Pages config on windows if it has admin rights, right-click the
.exe file and choose "Run as Administrator" the first time. If it suggests you reboot, then it has attempted
to set up the huge pages config in windows and it will hopefully work after a reboot. If it works, you can
now run the miner as a regular non-priveliged user. (Running as admin permanently is not recommended because
you never know whether a bug will be remotely exploitable (such as a malformed package).

If you want to set up large pages manually, you need to edit your system's group policies to enable
locking large pages. Here are the steps from MSDN

1. On the Start menu, click Run. In the Open box, type gpedit.msc.
2. On the Local Group Policy Editor console, expand Computer Configuration, and then expand Windows Settings.
3. Expand Security Settings, and then expand Local Policies.
4. Select the User Rights Assignment folder.
5. The policies will be displayed in the details pane.
6. In the pane, double-click Lock pages in memory.
7. In the Local Security Setting – Lock pages in memory dialog box, click Add User or Group.
8. In the Select Users, Service Accounts, or Groups dialog box, add an account that you will run the miner on
9. Reboot for change to take effect.

Windows also tends to fragment memory a lot. If you are running on a system with 4-8GB of RAM you might need
to switch off all the auto-start applications and reboot to have a large enough chunk of contiguous memory.
Having a big enough swapfile can help, since it allows the OS to defragement memory allocations and free up
contiguous ranges for large page allocations..


 ** Linux **
You will need to configure huge page support and increase your memlock limit (ulimit -l).

To enable hugepages support on your system, put the following in /etc/sysctl.conf:
vm.nr_hugepages                     = 64
vm.nr_overcommit_hugepages          = 128
vm.hugetlb_shm_group                = 200

nr_hugepages is the number of 2MB blocks, you need at least one per normal miner thread.
nr_overcommit_hugepages will allow the system to find more hugepages if a program requests them.
hugetlb_shm_group should be set to the group numer of a group the user running the miner is a member of.

Optional: increasing memlock limit, this normally has no effect on hashrate.
To increase memlock limit, put the following in /etc/security/limits.conf
* soft memlock 262144
* hard memlock 262144

Running the miner as root would ignore the group membership and memlock limits, but is NOT recommended
due to the lack of security this would result in.
