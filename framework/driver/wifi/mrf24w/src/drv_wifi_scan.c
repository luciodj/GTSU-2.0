/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_wifi_scan.c

  Summary:
    Module for Microchip TCP/IP Stack
    -Provides access to MRF24W WiFi controller
    -Reference: MRF24W Data sheet, IEEE 802.11 Standard

  Description:
    MRF24W Driver Scan Functions

 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) <2014> released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
//DOM-IGNORE-END

/*******************************************************************************
*                                    INCLUDES
********************************************************************************/
#include "tcpip/tcpip.h"

#if defined(WF_CS_TRIS) && defined(WF_USE_SCAN_FUNCTIONS)

#include "drv_wifi_mac.h"

/*******************************************************************************
*                                     INCLUDES
********************************************************************************/

/* used for assertions */
#if defined(WF_DEBUG)
    #define WF_MODULE_NUMBER    WF_MODULE_WF_SCAN
#endif

static bool WF_CMIsHostScanAllowed(void)
{
    uint8_t   profileID;
    uint8_t   profileIDState;

    WF_CMCheckConnectionState(&profileIDState, &profileID);
    if (profileIDState == WF_CSTATE_CONNECTION_IN_PROGRESS || profileIDState == WF_CSTATE_RECONNECTION_IN_PROGRESS)
        return false;

    return true;
}

/*******************************************************************************
  Function:
    uint16_t WF_Scan(uint8_t CpId)

  Summary:
    Commands the MRF24W to start a scan operation.  This will generate the
    WF_EVENT_SCAN_RESULTS_READY event.

  Description:
    Directs the MRF24W to initiate a scan operation utilizing the input
    Connection Profile ID.  The Host Application will be notified that the scan
    results are ready when it receives the WF_EVENT_SCAN_RESULTS_READY event.
    The eventInfo field for this event will contain the number of scan results.
    Once the scan results are ready they can be retrieved with
    WF_ScanGetResult().

    Scan results are retained on the MRF24W until:
    1.  Calling WF_Scan() again (after scan results returned from previous
        call).
    2.  MRF24W reset.

    MRF24WB0M & MRF24WG0M support up to max of 60 scan results (SSIDs).

  Precondition:
    MACInit must be called first.

  Parameters:
    CpId - Connection Profile to use.
            If the CpId is valid then the values from that Connection Profile
            will be used for filtering scan results.  If the CpId is set to
            WF_SCAN_ALL (0xFF) then a default filter will be used.

            Valid CpId
            * If CP has a defined SSID only scan results with that SSID are
               retained.
            * If CP does not have a defined SSID then all scanned SSID�s will be
               retained
            * Only scan results from Infrastructure or AdHoc networks are
               retained, depending on the value of networkType in the Connection Profile
            * The channel list that is scanned will be determined from
               channelList in the Connection Algorithm (which must be defined
               before calling this function).

            CpId is equal to WF_SCAN_ALL
            * All scan results are retained (both Infrastructure and Ad Hoc
               networks).
            * All channels within the MRF24W�s regional domain will be
               scanned.
            * No Connection Profiles need to be defined before calling this
               function.
            * The Connection Algorithm does not need to be defined before
               calling this function.

  Returns:
    Operation results. Success or Failure

  Remarks:
    Host scan is allowed only in idle or connected state.
    If MRF24W FW is in the midst of connection ( or reconnection) process, then
    host scan can hammer connection process, and furthermore it may cause
    fatal failure in MRF24W FW operation. To use host scan, we strongly
    recommend the user to disable MRF24W FW connection manager by enabling
    #define DISABLE_MODULE_FW_CONNECT_MANAGER_IN_INFRASTRUCTURE
    in drv_wifi_config.h
  *****************************************************************************/
uint16_t WF_Scan(uint8_t CpId)
{
    uint8_t   hdr[4];
#ifndef MRF24WG
    uint8_t   connectionState;
    uint8_t   dummy;
#endif

    /* WARNING !!! :
    * Host scan is allowed only in idle or connected state.
    * If module FW is in the midst of connection ( or reconenction) process, then
    * host scan can hammer connection process, and furthermore it may cause
    * fatal failure in module FW operation. To be safte to use host scan, we strongly
    * recommend you to disable module FW connection manager by uncommenting
    * #define DISABLE_MODULE_FW_CONNECT_MANAGER_IN_INFRASTRUCTURE
    * in drv_wifi_config.h
    */
    if (!WF_CMIsHostScanAllowed())
        return WF_ERROR_OPERATION_CANCELLED;

#ifndef MRF24WG
    WF_CMGetConnectionState(&connectionState, &dummy);
    if (connectionState == WF_CSTATE_NOT_CONNECTED)
        WF_CMConnect(0xff); /* 120c host scan bug workaround */
#endif

    hdr[0] = WF_MGMT_REQUEST_TYPE;
    hdr[1] = WF_SCAN_START_SUBTYPE;
    hdr[2] = CpId;                  /* Connection Profile ID */
    hdr[3] = 0;                     /* not used              */

    SendMgmtMsg(hdr,             /* header           */
                sizeof(hdr),     /* size of header   */
                NULL,            /* no data          */
                0);              /* no data          */

    /* wait for mgmt response, free it after it comes in (no data needed) */
    WaitForMgmtResponse(WF_SCAN_START_SUBTYPE, FREE_MGMT_BUFFER);

    return WF_SUCCESS;
}

/*******************************************************************************
  Function:
    void WF_ScanGetResult(uint8_t listIndex, tWFScanResult  *p_scanResult)

  Summary:
    Read scan results back from MRF24W.

  Description:
    After a scan has completed this function is used to read one or more of the
    scan results from the MRF24W.  The scan results will be written
    contiguously starting at p_scanResult (see tWFScanResult structure for
    format of scan result).
    MRF24WB0M & MRF24WG0M support up to max of 60 scan results (SSIDs).

  Precondition:
    MACInit must be called first.  WF_EVENT_SCAN_RESULTS_READY event must have
    already occurrerd.

  Parameters:
    listIndex - Index (0-based list) of the scan entry to retrieve.
    p_scanResult - Pointer to location to store the scan result structure

  Returns:
    None.

  Remarks:
      RSSI can only be obtained from the scan results p_scanResult->rssi.
      MRF24W checks out the signal strength from the preamble of the incoming packets.
      The higher the values, the stronger is the received signal strength.

      p_scanResult->rssi contains received signal strength indicator (RSSI).
      * MRF24WB : RSSI_MAX (200) , RSSI_MIN (106).
      * MRF24WG : RSSI_MAX (128) , RSSI_MIN (43).

       The RSSI value is not directly translated to dbm because this is not calibrated number.
       However, as a guideline, for 802.11b, MAX(200) corresponds to 0 dbm, MIN (106) corresponds to -94 dbm.

  *****************************************************************************/
void WF_ScanGetResult(uint8_t          listIndex,
                       tWFScanResult  *p_scanResult)
{

    uint8_t   hdr[4];
    /* char rssiChan[48]; */ /* reference for how to retrieve RSSI */

    hdr[0] = WF_MGMT_REQUEST_TYPE;
    hdr[1] = WF_SCAN_GET_RESULTS_SUBTYPE;
    hdr[2] = listIndex;        /* scan result index to read from */
    hdr[3] = 1;                /* number of results to read            */

    SendMgmtMsg(hdr,             /* header           */
                sizeof(hdr),     /* size of header   */
                NULL,            /* no data          */
                0);              /* no data          */

    /* index 4 contains number of scan results returned, index 5 is first byte of first scan result */
    WaitForMgmtResponseAndReadData(WF_SCAN_GET_RESULTS_SUBTYPE,         /* expected subtype               */
                                   sizeof(tWFScanResult),               /* num data bytes to read         */
                                   5,                                   /* starting at this index         */
                                  (uint8_t *)p_scanResult);               /* write the response data here   */


    /* fix up endianness on the two 16-bit values in the scan results */
    p_scanResult->beaconPeriod = WFSTOHS(p_scanResult->beaconPeriod);
    p_scanResult->atimWindow   = WFSTOHS(p_scanResult->atimWindow);

    /* reference for how to retrieve RSSI */
    /* Display SSID  & Channel */
    /* sprintf(rssiChan, "  => RSSI: %u, Channel: %u\r\n",  p_scanResult->rssi, p_scanResult->channel);  */
    /* putsUART(rssiChan); */
}

#endif /* WF_CS_TRIS && WF_USE_SCAN_FUNCTIONS */
