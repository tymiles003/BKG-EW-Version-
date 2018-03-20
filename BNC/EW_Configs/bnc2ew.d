#
# gsof2ew configuration file
#

ModuleId        MOD_K2EW        # module id for this import
RingName        WAVE_RING       # transport ring to use for input/output

LogFile         0               # If 0, don't output to logfile; if 1, do
                                # if 2, log to module log but not stderr/stdout

HeartbeatInt    30              # Heartbeat interval in seconds

Network         PR              # Network name for EW msg headers

Debug         2                 # debug level: 0 or commented out for no debug
                                # logging; 1 - 4 for increasing verbosity
                                # default: no debug output

SampRate       1                # GPS Samplerate in Hz

SubX                            # If you prefer the N
SubY                            # If you prefer the E
SubZ                            # If you prefer the U
