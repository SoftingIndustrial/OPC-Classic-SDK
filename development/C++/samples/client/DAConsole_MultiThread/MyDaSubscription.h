#ifndef _MYDASUBSCRIPTION_H_
#define _MYDASUBSCRIPTION_H_

#ifdef TBC_OS_WINDOWS
#include "Da\ClientDaSubscription.h"
#endif
#include "ClientApplication.h"

#ifdef TBC_OS_LINUX
#include "ClientDaSubscription.h"
#endif
#include "Mutex.h"

extern bool g_tracesOn;
extern Mutex g_syncDisplay;
extern const unsigned int g_itemsCountPerSubscription;

extern unsigned long g_dataChangeSetsReceived;
extern unsigned long g_totalGoodDataChanges;
extern unsigned long g_totalBadDataChanges;
extern unsigned long g_legitGoodDataChanges;
extern unsigned long g_legitBadDataChanges;
extern int g_random;

class MyDaSubscription :
	public SoftingOPCToolboxClient::DaSubscription
{
private:
	unsigned int m_subNo;

	ULONGLONG* m_badItems;
	ULONGLONG* m_goodItems;
	DWORD* m_badItemsCount;
	DWORD* m_goodItemsCount;
	std::vector<DaItem*> m_daItemsListReserve;

public:

	MyDaSubscription(unsigned int subNo, unsigned long updateRate, MyDaSession* parentSession)
		: DaSubscription(updateRate, parentSession)
	{
		m_badItems = (ULONGLONG*)malloc(sizeof(ULONGLONG) * g_itemsCountPerSubscription);
		m_goodItems = (ULONGLONG*)malloc(sizeof(ULONGLONG) * g_itemsCountPerSubscription);
		m_badItemsCount = (DWORD*)malloc(sizeof(DWORD) * g_itemsCountPerSubscription);
		m_goodItemsCount = (DWORD*)malloc(sizeof(DWORD) * g_itemsCountPerSubscription);
		m_subNo = subNo;
	}

	void handleStateChangeCompleted(EnumObjectState state)
	{
		if (g_tracesOn == true)
		{
			if (g_syncDisplay.lock(0))
			{
				tstring stateToString;

				switch (state)
				{
				case EnumObjectState_DISCONNECTED:
					stateToString = _T("DISCONNECTED");
					break;

				case EnumObjectState_CONNECTED:
					stateToString = _T("CONNECTED");
					break;

				case EnumObjectState_ACTIVATED:
					stateToString = _T("ACTIVATED");
				} //end switch

				_tprintf(_T("%s state changed - %s\n"), _T("Subscription"), stateToString.c_str());
				g_syncDisplay.unlock();
			}
		}
		else if (m_subNo % g_silentDisplayCount == 0)
		{
			_tprintf(_T("sub%dStChCompl "), m_subNo);
		}
	} //end handleStateChangeCompleted


	void handlePerformStateTransitionCompleted(
		unsigned long executionContext,
		long result)
	{
		if (SUCCEEDED(result))
		{
			if (g_tracesOn == true)
			{
				if (g_syncDisplay.lock(0))
				{
					_tprintf(_T("\n"));
					_tprintf(_T("Subscription %d performed state transition - context: %lu\n"), m_subNo, executionContext);
					g_syncDisplay.unlock();
				} //end if
			}
			else if ((result != S_OK) || (m_subNo % g_silentDisplayCount == 0))
			{
				if (g_syncDisplay.lock(0))
				{
					_tprintf(_T("sub%dProgStTrCompl(%d) "), m_subNo, result);
					g_syncDisplay.unlock();
				}
			}
		}
		else
		{
			if (g_syncDisplay.lock(0))
			{
				_tprintf(_T("Subscription %d performed state transition failed - [0x%8.8X]"), m_subNo, result);
				_tprintf(_T("sub%dProgStTrCompl(%d) "), m_subNo, result);
				g_syncDisplay.unlock();
			}
		}
	} //handlePerformStateTransitionCompleted


	void handleDataChanged(
		const std::vector<DaItem*>& items,
		const std::vector<ValueQT*>& values,
		const std::vector<long>& results)
	{
		memset(m_badItems, 0, sizeof(ULONGLONG) * g_itemsCountPerSubscription);
		memset(m_goodItems, 0, sizeof(ULONGLONG) * g_itemsCountPerSubscription);
		memset(m_badItemsCount, 0, sizeof(DWORD) * g_itemsCountPerSubscription);
		memset(m_goodItemsCount, 0, sizeof(DWORD) * g_itemsCountPerSubscription);

		++g_dataChangeSetsReceived;
		unsigned long i;
		size_t count = items.size();

		int place;
		for (i = 0; i < count; i++)
		{
			place = ((MyDaItem*)items[i])->getItemNo() % g_itemsCountPerSubscription;
			if (values[i]->getQuality() != EnumQuality_GOOD)
			{
				++m_badItemsCount[place];
				if (m_badItems[place] < values[i]->getTimeStamp().getMicroseconds())
				{
					m_badItems[place] = values[i]->getTimeStamp().getMicroseconds();
				}
			}
			else
			{
				++m_goodItemsCount[place];
				if (m_goodItems[place] < values[i]->getTimeStamp().getMicroseconds())
				{
					m_goodItems[place] = values[i]->getTimeStamp().getMicroseconds();
				}
			}
		}

		DWORD totalm_goodItems = 0;
		DWORD totalm_badItems = 0;
		DWORD totalGoodLegitItems = 0;
		DWORD totalBadLegitItems = 0;
		for (i = 0; i < g_itemsCountPerSubscription; ++i)
		{
			totalm_goodItems += m_goodItemsCount[i];
			totalm_badItems += m_badItemsCount[i];
			if (m_badItems[i] > m_goodItems[i])
			{
				++totalBadLegitItems;
			}
			else
			{
				if (m_goodItems[i] > 0)
				{
					++totalGoodLegitItems;
				}
			}
		}

		if (g_syncDisplay.lock(0))
		{
			if (g_tracesOn == true)
			{
				_tprintf(_T("\n%s - Data changed\n"), _T("Subscription"));

				for (i = 0; i < count; i++)
				{
					_tprintf(_T("%-23.23s - %s\n"), items[i]->getId().c_str(), values[i]->toString().c_str());
				} //end for
			}
			else
			{
				if (totalm_badItems > 0)
				{
					_tprintf(_T("\a\n"));
					char buffer[256];
					_stprintf_s(buffer, _T("sub%dDC(t:%dtb:%dtg:%d,lg:%d,lb:%d) "), m_subNo, items.size(), totalm_badItems, totalm_goodItems, totalGoodLegitItems, totalBadLegitItems);
					_tprintf(buffer);
					getApplication()->trace(EnumTraceLevel_INF, EnumTraceGroup_USER1, "MyDaSubscription", buffer);
				}
				else
				{
					_tprintf(_T("sub%dDC(t:%d,tl:%d) "), m_subNo, items.size(), totalGoodLegitItems);
				}
			}
			g_syncDisplay.unlock();
		}


		g_totalGoodDataChanges += totalm_goodItems;
		g_totalBadDataChanges += totalm_badItems;
		g_legitGoodDataChanges += totalGoodLegitItems;
		g_legitBadDataChanges += totalBadLegitItems;
	}

	void handleReadCompleted(
		unsigned long executionContext,
		const std::vector<DaItem*>& items,
		const std::vector<ValueQT*>& values,
		const std::vector<long>& results,
		long result)
	{
		if (SUCCEEDED(result))
		{
			if (g_tracesOn == true)
			{
				if (g_syncDisplay.lock(0))
				{
					unsigned long i;
					size_t itemCount = items.size();
					_tprintf(_T("\nSubscription - Read completed\n"));

					for (i = 0; i < itemCount; i++)
					{
						tstring id = items[i]->getId();

						if (SUCCEEDED(results[i]))
						{
							_tprintf(_T("%-23.23s - %s\n"), id.c_str(), values[i]->toString().c_str());
						}
						else
						{
							_tprintf(_T("Read failed - %s [0x%8.8X]\n"), id.c_str(), (unsigned int)results[i]);
						}//end if...else
					}//end for
					g_syncDisplay.unlock();
				}
			}
			else if ((result != S_OK) || (m_subNo % g_silentDisplayCount == 0))
			{
				if (g_syncDisplay.lock(0))
				{
					_tprintf(_T("sub%dRd%dItmCompl(%d) "), m_subNo, items.size(), result);
					g_syncDisplay.unlock();
				}
			}
		}
		else
		{
			if (g_syncDisplay.lock(0))
			{
				if (g_tracesOn == true)
				{
					_tprintf(_T("Read %d items failed - [0x%8.8X]\n"), items.size(), (unsigned int)result);
				}
				else
				{
					_tprintf(_T("sub%dRd%dItmCompl(%d) "), m_subNo, items.size(), result);
				}
				g_syncDisplay.unlock();
			}
		}//end if...else
	} //end handleReadCompleted

	void  handleWriteCompleted(
		unsigned long executionContext,
		const std::vector<DaItem*>& items,
		const std::vector<ValueQT*>& values,
		const std::vector<long>& results,
		long result)
	{

		if (SUCCEEDED(result))
		{
			if (g_tracesOn == true)
			{
				if (g_syncDisplay.lock(0))
				{
					unsigned long i;
					size_t itemCount = items.size();
					_tprintf(_T("\nSubscription - Write completed\n"));

					for (i = 0; i < itemCount; i++)
					{
						tstring id = items[i]->getId();

						if (SUCCEEDED(results[i]))
						{
							_tprintf(_T("%-23.23s - %s\n"), id.c_str(), values[i]->toString().c_str());
						}
						else
						{
							_tprintf(_T("Write failed - %s [0x%8.8X]\n"), id.c_str(), (unsigned int)results[i]);
						}//end if...else
					}//end for
					g_syncDisplay.unlock();
				}
			}
			else if ((result != S_OK) || (m_subNo % g_silentDisplayCount == 0))
			{
				if (g_syncDisplay.lock(0))
				{
					_tprintf(_T("sub%dWr%dItmCompl(%d) "), m_subNo, items.size(), result);
					g_syncDisplay.unlock();
				}
			}
		}
		else
		{
			if (g_syncDisplay.lock(0))
			{
				if (g_tracesOn == true)
				{
					_tprintf(_T("Write %d items failed - [0x%8.8X]\n"), items.size(), (unsigned int)result);
				}
				else
				{
					_tprintf(_T("sub%dWr%dItmCompl(%d) "), m_subNo, items.size(), result);
				}
				g_syncDisplay.unlock();
			}
		}//end if...else
	} //end handleWriteCompleted

	std::vector<DaItem*> removeDaItems(unsigned int &count)
	{
		std::vector<DaItem*> currentItems = getItemList();
		std::vector<DaItem*>::iterator it;
		std::vector<DaItem*> removedItems;
		unsigned int itemNo;
		bool found = false;
		unsigned int i, j, n;
		if (count > currentItems.size())
		{
			count = count % currentItems.size();
		}
		removedItems.assign(count, NULL);
		for (i = 0; i < count; ++i)
		{
			do
			{
				itemNo = g_random % currentItems.size();
				found = false;
				for (j = 0; j < i; ++j)
				{
					if (removedItems[j] == currentItems[itemNo])
					{
						found = true;
					}
				}
			} while (found = true);
			m_daItemsListReserve.push_back(currentItems[itemNo]);
			removedItems[i] = currentItems[itemNo];
		}
		for (i = 0; i < count; ++i)
		{
			removeDaItem(currentItems[itemNo]);
			for (it = currentItems.begin(); it != currentItems.end(); ++it)
			{
				if (*it == currentItems[itemNo])
				{
					currentItems.erase(it);
					break;
				}
			}
		}
		return removedItems;
	}

	std::vector<DaItem*> addDaItems(unsigned int &count)
	{
		std::vector<DaItem*> addedItems;
		unsigned int itemNo;
		MyDaItem *currItem;
		if (count > m_daItemsListReserve.size())
		{
			count = count % m_daItemsListReserve.size();
		}
		addedItems.assign(count, NULL);
		for (unsigned int i = 0; i < count; ++i)
		{
			itemNo = g_random % m_daItemsListReserve.size();
			currItem = (MyDaItem*)m_daItemsListReserve[itemNo];
			for (std::vector<DaItem*>::iterator it = m_daItemsListReserve.begin(); it != m_daItemsListReserve.end(); ++it)
			{
				if (currItem == *it)
				{
					currItem = new MyDaItem(currItem->getItemNo(), currItem->getItemId(), this, currItem->getDataType());
					addedItems[i] = currItem;
					delete *it;
					m_daItemsListReserve.erase(it);
					break;
				}
			}
		}
		return addedItems;
	}

	unsigned int getSubNo()
	{
		return m_subNo;
	}

	virtual ~MyDaSubscription()
	{
		free(m_badItems);
		free(m_goodItems);
		free(m_badItemsCount);
		free(m_goodItemsCount);
		for (std::vector<DaItem*>::iterator it = m_daItemsListReserve.begin(); it != m_daItemsListReserve.end(); ++it)
		{
			delete *it;
		}
		m_daItemsListReserve.clear();
	} // end dtor

};  // end class MyDaSubscription

#endif
