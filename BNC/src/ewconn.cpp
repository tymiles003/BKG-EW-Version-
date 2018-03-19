#include "ewconn.h"

EWconn::EWconn(QObject *parent) : QObject(parent), BeatHeart(new QTimer)
{
    connected = false;
}

void EWconn::setConfig(QString configfile)
{
    config = configfile;
}

void EWconn::setPid(qint64 mypid)
{
    pid = mypid;
}

bool EWconn::isConn()
{
    return connected;
}

/* Get EW configuration         */
int EWconn::get_config(char *configfile)
{
    int      nfiles;
    char     init[6];

    /* Open the main configuration file
     * ********************************/
    nfiles = k_open( configfile );
    if ( nfiles == 0 )
    {
        qDebug() << "Error opening configuration file";
        return -1;
    }

    /* Process all nested configuration files
     * **************************************/
    while ( nfiles > 0 )          /* While there are config files open */
    {
        while ( k_rd() )           /* Read next line from active file  */
        {
            int  success;
            char *com;
            char *str;

            com = k_str();          /* Get the first token from line */

            if ( !com ) continue;             /* Ignore blank lines */
            if ( com[0] == '#' ) continue;    /* Ignore comments */

            /* Open another configuration file
             * *******************************/
            if ( com[0] == '@' )
            {
                success = nfiles + 1;
                nfiles  = k_open( &com[1] );
                if ( nfiles != success )
                {
                    qDebug() << "Error opening command file" << QString(com[1]);
                    return -1;
                }
                continue;
            }
            /* Read configuration parameters
             * *****************************/

            else if ( k_its( "ModuleId" ) )
            {
                unsigned char gcfg_module_idnum;
                if ( (str = k_str()) != 0 )
                {
                    /* copy module name; make sure NULL terminated */
                    mod_name = QString(str);
                    if ( GetModId(str, &gcfg_module_idnum) == -1 )
                    {
                        qDebug() << "Invalid ModuleId Please Register ModuleId <%s> in earthworm.d!";
                        return -1;
                    }
                    mod_id = gcfg_module_idnum;
                }
                init[0] = 1;
            }

            else if ( k_its( "RingName" ) )
            {
                long gcfg_ring_key;
                if ( (str = k_str()) != NULL )
                {
                    /* copy ring name; make sure NULL terminated */
                    ring_name = QString(str);
                    if ( (gcfg_ring_key = GetKey(str)) == -1 )
                    {
                        qDebug() << "Invalid RingName" ;
                        return -1;
                    }
                    ring_id = gcfg_ring_key;
                }
                init[1] = 1;
            }

            else if ( k_its( "HeartbeatInt" ) )
            {
                heartbeat = k_int();
                init[2] = 1;
            }

            else if ( k_its( "SampRate" ) )
            {
                sampler = k_int();
                init[3] = 1;
            }

            else if ( k_its( "SubX" ) )
            {
                SubX = k_int();
                Xcor = true;
            }
            else if ( k_its( "SubY" ) )
            {
                SubY = k_int();
                Ycor = true;
            }
            else if ( k_its( "SubZ" ) )
            {
                SubZ = k_int();
                Ycor = true;
            }

            else if ( k_its( "LogFile" ) )
            {
                logf = k_int();
                init[4] = 1;
            }

            /* Optional Commands */
            else if ( k_its( "Network" ) )
            {
                netID = QString(k_str());
                init[5] = 1;
            }

            else if (k_its( "Debug" ) )
            {
                debug = k_int();
                init[6] = 1;
            }
            else if (k_its( "InjectVel" ) )
            {
                velocity = true;
            }

            else
            {
                /* An unknown parameter was encountered */
                qDebug() << "unknown parameter in GPS Config File";
                return -1;
            }

            /* See if there were any errors processing the command
       ***************************************************/
            if ( k_err() )
            {
                qDebug() << "Bad <%s> command in GPS Config File";
                return -1;
            }
        }
        nfiles = k_close();
    }

    /* After all files are closed, check flags for missed required commands
           ***********************************************************/
    int nmiss = 0;
    for ( int i = 0; i < 6; i++ )
        if ( !init[i] )
            nmiss++;

    if ( nmiss > 0 )
    {
        qDebug() << "ERROR, no ";
        if ( !init[0] ) qDebug() <<"<ModuleId>";
        if ( !init[1] ) qDebug() <<"<RingName>";
        if ( !init[2] ) qDebug() <<"<HeartbeatInt>";
        if ( !init[3] ) qDebug() <<"<SampRate>";
        if ( !init[4] ) qDebug() <<"<LogFile> ";
        if ( !init[5] ) qDebug() <<"<Network> ";
        if ( !init[6] ) qDebug() <<"<Debug> ";
        qDebug() <<"configuration detected";
        return -1;
    }
    return 0;
}

/* Append stuff to log WIP      */
void EWconn::appendlog(QString status)
{
    if(debug == 1 || debug == 2)
        qDebug() << status;
}

/* Process GPS State            */
void EWconn::processState(QByteArray staID , bncTime time, QVector<double> xx)
{
    if (debug == 2){
        double xRover, yRover, zRover, N, E, U;
        xRover = xx.at(0);
        yRover = xx.at(1);
        zRover = xx.at(2);
        N = xx.at(3);
        E = xx.at(4);
        U = xx.at(5);
        qDebug() << "Hello There";
        qDebug() << staID ;
        qDebug() << xRover << yRover << zRover;
        qDebug() << N << E << U;

    }
    createTracePacket(staID, time, xx);
}

/* Send Heartbeat Packet        */
void EWconn::sendHB()
{
    createHBPacket(TypeHeartBeat, 0, NULL);
}

/* Create Trace Packet          */
void EWconn::createTracePacket(QByteArray staID, bncTime mytime, QVector<double> myvector)
{
    for (int i=0; i < 3; i++){
        static int cd;
        char* chan;
        double xval, yval, zval;
        double nval, eval, uval;
        QString station(staID);
        station.resize(4);

        if (i == 0){
            chan  = "GPX";
            xval  = myvector.at(0);
            nval  = myvector.at(3);
        }
        if (i == 1){
            chan  = "GPY";
            yval  = myvector.at(1);
            eval  = myvector.at(4);
        }
        if (i == 2){
            chan  = "GPZ";
            zval = myvector.at(2);
            uval = myvector.at(5);
        }

        TracePacket ew_trace_pkt;
        MSG_LOGO logo;
        logo.type = TypeTraceBuf2;
        logo.mod = mod_id;
        logo.instid = InstId;
        memset(&ew_trace_pkt,0,sizeof(ew_trace_pkt));
        strncpy(ew_trace_pkt.trh2.sta,station.toLocal8Bit().data(), TRACE2_STA_LEN-1);
        ew_trace_pkt.trh2.version[0]=TRACE2_VERSION0;
        ew_trace_pkt.trh2.version[1]=TRACE2_VERSION1;
        strcpy(ew_trace_pkt.trh2.datatype,"i4");   /* enter data type (Intel ints) */
        ew_trace_pkt.trh2.samprate = (double) sampler; /* enter GPS sample rate */
        ew_trace_pkt.trh2.nsamp = sampler;   /* enter GPS number of samples rate */

        if (sizeof(TRACE2_HEADER) + ew_trace_pkt.trh2.nsamp * sizeof(int32_t) > MAX_TRACEBUF_SIZ)
        {
            /* exit thread function */
            qDebug() << "There will be an error";
            return;
        }

        strncpy(ew_trace_pkt.trh2.chan, chan, TRACE2_CHAN_LEN-1);
        ew_trace_pkt.trh2.chan[TRACE2_CHAN_LEN-1] = '\0';

        strncpy(ew_trace_pkt.trh2.net,netID.toLocal8Bit().data(), TRACE2_NET_LEN-1);
        ew_trace_pkt.trh2.loc[TRACE2_LOC_LEN-1] = '\0';

        strncpy(ew_trace_pkt.trh2.loc,"--", TRACE2_LOC_LEN-1);
        ew_trace_pkt.trh2.loc[TRACE2_LOC_LEN-1] = '\0';

        unsigned int Day,Month,Year;
        unsigned int Hour, Minute;
        double Seconds;
        mytime.civil_date(Year,Month,Day);
        mytime.civil_time(Hour,Minute,Seconds);
        int second = (int) Seconds;
        int milli  =  (int) ((Seconds - second) * 1000);
        QDateTime timeofobs;
        timeofobs.setTimeSpec(Qt::TimeSpec::UTC); // THIS ONE IS IMPORTANT
        timeofobs.setTime(QTime(Hour,Minute,second,milli));
        timeofobs.setDate(QDate(Year,Month,Day));
        double starttime = (double) timeofobs.toTime_t();

        /* calculate and enter start-timestamp for packet */
        ew_trace_pkt.trh2.starttime = starttime;

        /* endtime is the time of last sample in this packet, not the time *
         * of the first sample in the next packet */
        ew_trace_pkt.trh2.endtime = ew_trace_pkt.trh2.starttime + (double)(ew_trace_pkt.trh2.nsamp - 1) / ew_trace_pkt.trh2.samprate;

        if (i == 0){
            double tempX = xval * 1000; // Convert from m to mm
            if(Xcor)
                tempX = nval * 1000; // Convert from m to mm
            int32_t X = (int32_t) tempX;
            /* copy payload of 32-bit ints into trace buffer (after header) */
            memcpy(&ew_trace_pkt.msg[sizeof(TRACE2_HEADER)],&X , ew_trace_pkt.trh2.nsamp*sizeof(int32_t));
        }
        if (i == 1){
            double tempY = yval* 1000; // Convert from m to mm
            if(Ycor)
                tempY = eval * 1000; // Convert from m to mm
            int32_t Y = (int32_t) tempY;
            /* copy payload of 32-bit ints into trace buffer (after header) */
            memcpy(&ew_trace_pkt.msg[sizeof(TRACE2_HEADER)],&Y, ew_trace_pkt.trh2.nsamp*sizeof(int32_t));
        }
        if (i == 2){
            double tempZ = zval * 1000; // Convert from m to mm
            if(Zcor)
                tempZ = uval * 1000; // Convert from m to mm
            int32_t Z = (int32_t) Z;
            /* copy payload of 32-bit ints into trace buffer (after header) */
            memcpy(&ew_trace_pkt.msg[sizeof(TRACE2_HEADER)],&Z, ew_trace_pkt.trh2.nsamp*sizeof(int32_t));
        }

        sleep_ew(10);/* Take a short nap so we don't flood the transport ring */


        /* send data trace message to Earthworm */
        if ( (cd = tport_putmsg(&region, &logo,
                                (int32_t)sizeof(TRACE2_HEADER) + (int32_t)ew_trace_pkt.trh2.nsamp * sizeof(int32_t),
                                (char *)&ew_trace_pkt)) != PUT_OK)
        {
            qDebug() << "There has been an error";
            return;
        }
    }
}

/* Create Heartbeat Packet      */
void EWconn::createHBPacket(unsigned char type,short code, char* message )
{
    char          outMsg[MAX_MSG_SIZE]; /* The outgoing message.        */
    time_t        msgTime;              /* Time of the message.         */

    /*  Get the time of the message                                     */
    time( &msgTime );

    /*  Build & process the message based on the type                     */
    if ( TypeHeartBeat == type )
    {
      sprintf( outMsg, "%ld %d\n", (long) msgTime,(int) pid );

      /*Write the message to the output region                            */
      if ( tport_putmsg( &region, &hblogo, (long) strlen(outMsg), outMsg ) != PUT_OK )
      {
        /*     Log an error message                                       */
        appendlog("Failed to send a heartbeat message");
      }
    }
    else
    {
      if ( message )
      {
          sprintf( outMsg, "%ld %hd %s\n", (long) msgTime, code, message );
          appendlog("Error:"+QString(message));
      }
      else
      {
          sprintf( outMsg, "%ld %hd\n", (long) msgTime, code );
          appendlog("Error: (No description)");
      }

      MSG_LOGO error;
      error.instid = InstId;
      error.mod    = mod_id;
      error.type   = TypeError;
      /*Write the message to the output region                         */
      if ( tport_putmsg( &region, &error, (long) strlen( outMsg ), outMsg ) != PUT_OK )
      {
        appendlog("Failed to send an error message");
      }
    }
}

/* Connect to Earthworm         */
int EWconn::connectToEw() {
    if(!config.isEmpty() && pid != 0){

        // Set Stuff
        char *runPath;

        appendlog(QString("using default config file: ") + QString(config));

        /* Change working directory to environment variable EW_PARAMS value
            *********************************************************************/
        runPath = getenv( "EW_PARAMS" );

        if ( runPath == NULL ) {
            appendlog("status: Environment variable EW_PARAMS not defined.");
            appendlog(" Exiting.\n");
            return -1;
        }

        if ( *runPath == '\0' ) {
            appendlog("status: Environment variable EW_PARAMS ");
            appendlog("defined, but has no value. Exiting.\n");
            return -1;
        }

        if ( chdir_ew( runPath ) == -1 ) {
            appendlog(QString("status: Params directory not found: ") + QString(runPath) + "\n");
            appendlog(QString("status: Reset environment variable EW_PARAMS."));
            appendlog(QString("Exiting.\n"));
            return -1;
        }

        /* Look up ids in earthworm.d tables
               ***********************************/
        if ( GetLocalInst( &InstId ) != 0 ) {
            appendlog("status: error getting local installation id; exiting!\n");
            return -1;
        }

        /* setup logo type values for Eartworm messages */
        if(GetType("TYPE_ERROR", &TypeError) != 0 ||
           GetType("TYPE_HEARTBEAT",&TypeHeartBeat) != 0 ||
           GetType("TYPE_TRACEBUF2",&TypeTraceBuf2) != 0)
        {
          /* error fetching logo type values; show error message and abort */
          appendlog("Error fetching logo type values for EW messages");
          return  -1;
        }

        if( get_config(config.toLatin1().data()) == -1)
            return -1;

        // Define a heartbeat
        hblogo.instid = InstId;
        hblogo.mod    = mod_id;
        hblogo.type   = TypeHeartBeat;

        /* Attach to shared memory ring
            *****************************/
        tport_attach( &region, ring_id );

        /* Start beating our heart */
        BeatHeart->setInterval(heartbeat*1000);
        connect(BeatHeart,SIGNAL(timeout()),this,SLOT(sendHB()));
        BeatHeart->start();

        // Report connected
        connected = true;

        return 0;
    }
    return -1;
}

/* Disconnect From Earthworm    */
int EWconn::disconnectFromEw(){
    if (connected){
        tport_detach( &region );
        appendlog("Successful Disconnection");
        connected = false;
        return 0;
    } else {
        connected = false;
        appendlog("Not Connected");
        return 0;
    }
}
