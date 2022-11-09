#include <udt.h>
#include <ccc.h>
#include <iostream>
using namespace std;

class CTCP: public CCC
{
public:
   void init()
   {
      printf("CTCP init\n");
      m_bSlowStart = true;
      m_issthresh = 83333;

      m_dPktSndPeriod = 0.0;
      m_dCWndSize = 2.0;

      setACKInterval(2);
      setRTO(1000000);
   }

   virtual void onACK(const int& ack)
   {
      if (ack == m_iLastACK)
      {
      	 printf("dup ack\n");
         if (3 == ++ m_iDupACKCount)
            DupACKAction();
         else if (m_iDupACKCount > 3)
            m_dCWndSize += 1.0;
         else
            ACKAction();
      }
      else
      {
         if (m_iDupACKCount >= 3)
            m_dCWndSize = m_issthresh;

         m_iLastACK = ack;
         m_iDupACKCount = 1;

         ACKAction();
      }
   }

   virtual void onTimeout()
   {
      m_issthresh = getPerfInfo()->pktFlightSize / 2;
      if (m_issthresh < 2)
         m_issthresh = 2;

      m_bSlowStart = true;
      m_dCWndSize = 2.0;
   }

protected:
   virtual void ACKAction()
   {
      if (m_bSlowStart)
      {
         m_dCWndSize += 1.0;

         if (m_dCWndSize >= m_issthresh)
            m_bSlowStart = false;
      }
      else
         m_dCWndSize += 1.0/m_dCWndSize;
   }

   virtual void DupACKAction()
   {
      m_bSlowStart = false;

      m_issthresh = getPerfInfo()->pktFlightSize / 2;
      if (m_issthresh < 2)
         m_issthresh = 2;

      m_dCWndSize = m_issthresh + 3;
   }

protected:
   int m_issthresh;
   bool m_bSlowStart;

   int m_iDupACKCount;
   int m_iLastACK;
};

/*****************************************************************************
BiC TCP congestion control
Reference:
Lisong Xu, Khaled Harfoush, and Injong Rhee, "Binary Increase Congestion 
Control for Fast Long-Distance Networks", INFOCOM 2004.
*****************************************************************************/

class CBiCTCP: public CTCP
{
public:
   CBiCTCP()
   {
      printf("CBicTCP init\n");
      m_dMaxWin = m_iDefaultMaxWin;
      m_dMinWin = m_dCWndSize;
      m_dTargetWin = (m_dMaxWin + m_dMinWin) / 2;
      max_probe = false;

      m_dSSCWnd = 1.0;
      m_dSSTargetWin = m_dCWndSize + 1.0;
   }
   
   virtual void onTimeout()
   {
      m_issthresh = getPerfInfo()->pktFlightSize / 2;

      //m_dMaxWin = getPerfInfo()->pktFlightSize;
      m_dMaxWin = m_iDefaultMaxWin;
      if (m_issthresh < 2)
         m_issthresh = 2;
      
      m_dCWndSize = 2.0;
      
      // reset paramenter
      
      m_dMinWin = m_dCWndSize;

      max_probe = false;
      m_bSlowStart = true;
   }

protected:
   virtual void ACKAction()
   {
      if (m_dCWndSize < m_iLowWindow) // cwnd < low window 則 TCP reno
      {
         if (m_bSlowStart)
         {
            m_dCWndSize += 1.0;

            if (m_dCWndSize >= m_issthresh)
               m_bSlowStart = false;
         }
         else
            m_dCWndSize += 1/m_dCWndSize;

         return;
      }
      m_dMinWin = m_dCWndSize;
      if(m_dCWndSize < m_dMaxWin && !max_probe) // additive increase or binary increase
      {
         bic_inc = (m_dMaxWin - m_dMinWin)/2;
         
         if(bic_inc >= m_iSMax)      // 大於 Smax -> + Smax
            bic_inc = m_iSMax;
         else if (bic_inc <= m_iSMin)// 小於 Smin -> + Smin
            bic_inc = m_iSMin;

         m_dCWndSize += bic_inc/m_dCWndSize;
      }
      else if(m_dCWndSize >= m_dMaxWin && !max_probe)         // 如果 cwnd >= Winmax 則開始 max probe
      {
         max_probe = true;
         m_dSSCWnd = 1.0;
         m_dSSTargetWin = m_dCWndSize + 1.0;
         m_dMaxWin = m_iDefaultMaxWin;
      }
      if(max_probe)
      {
         m_dCWndSize += m_dSSCWnd/m_dCWndSize;
         if(m_dCWndSize >= m_dSSTargetWin)
         {
            m_dSSCWnd *= 2;
            m_dSSTargetWin = m_dCWndSize + m_dSSCWnd;
         }
         if(m_dSSCWnd >= m_iSMax)
            max_probe = false;
      }
      /*
      if (!max_probe)
      {
      	 if ((m_dTargetWin - m_dCWndSize < m_iSMin) && (m_dTargetWin - m_dCWndSize > 0))
      	 	m_dCWndSize += m_iSMin/m_dCWndSize;
         else if ((m_dTargetWin - m_dCWndSize < m_iSMax) && (m_dTargetWin - m_dCWndSize > 0))
            m_dCWndSize += (m_dTargetWin - m_dCWndSize)/m_dCWndSize;
         else 
            m_dCWndSize += m_iSMax/m_dCWndSize;

         if (m_dMaxWin > m_dCWndSize)
         {
            m_dMinWin = m_dCWndSize;
            m_dTargetWin = (m_dMaxWin + m_dMinWin) / 2;
         }
         else
         {
            max_probe = true;
            m_dSSCWnd = 1.0;
            m_dSSTargetWin = m_dCWndSize + 1.0;
            m_dMaxWin = m_iDefaultMaxWin;
         }
      }
      else
      {
         
         if(m_dCWndSize >= m_dSSTargetWin)
         {
            m_dSSCWnd *= 2;
            m_dSSTargetWin = m_dCWndSize + m_dSSCWnd;
         }
         if(m_dSSCWnd >= m_iSMax)
            max_probe = false;
      } */
   }

   virtual void DupACKAction()
   {
      printf("enter dup acktion\n");
      if (m_dCWndSize < m_iLowWindow) // if in TCP reno reset all parameter
      {
         m_issthresh = getPerfInfo()->pktFlightSize / 2;

         if (m_issthresh < 2)
            m_issthresh = 2;

         m_dCWndSize = m_issthresh + 3;

         // reset parameter
         max_probe = false;
         m_bSlowStart = false;         // 3 dup ACK 進入 congestion avoidance
         //m_dMaxWin = m_iLowWindow;
         m_dMinWin = m_dCWndSize;
      }
      else
      {
         m_dPreMax = m_dMaxWin;
         if(m_dPreMax == m_iDefaultMaxWin)
            m_dPreMax = 0;
         m_dMaxWin = m_dCWndSize;
         m_dCWndSize *= 0.875;
         m_dMinWin = m_dCWndSize;

         m_issthresh = m_dCWndSize;
         
         max_probe = false;
         m_bSlowStart = false;         // 3 dup ACK 進入 congestion avoidance
         
         if (m_dPreMax > m_dMaxWin)
         {
         	m_dMaxWin = (m_dMaxWin + m_dMinWin) / 2;
         }
      }
   }

private:
   static const int m_iLowWindow = 38;
   static const int m_iSMax = 32;
   static const int m_iSMin = 1;
   static const int m_iDefaultMaxWin = 1 << 29;
   
   bool max_probe;                        // 超時或是3 dup ACK 要重設該變數
   double bic_inc;
   double m_dMaxWin;                      // 超時或是3 dup ACK 要重設該變數
   double m_dMinWin;                      // 超時或是3 dup ACK 要重設該變數
   double m_dPreMax;
   double m_dTargetWin;
   double m_dSSCWnd;
   double m_dSSTargetWin;                
};

