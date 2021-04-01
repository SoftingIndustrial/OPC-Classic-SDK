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
extern MyMutex g_syncDisplay;

extern unsigned long g_dataChangeSetsReceived;
extern unsigned long g_totalGoodDataChanges;
extern unsigned long g_totalBadDataChanges;
extern unsigned long g_legitGoodDataChanges;
extern unsigned long g_legitBadDataChanges;
extern const unsigned long g_maxItemsPerSubscription;
extern const unsigned long g_minItemsPerSubscription;
extern const BOOL g_connectItemsFirst;

class MyDaSubscription :
	public SoftingOPCToolboxClient::DaSubscription
{
private:
	unsigned int m_subNo;

	ULONGLONG* m_badItems;
	ULONGLONG* m_goodItems;
	DWORD* m_badItemsCount;
	DWORD* m_goodItemsCount;
	MyMutex m_janitor;

public:

	MyDaSubscription(unsigned int subNo, unsigned long updateRate, MyDaSession* parentSession)
		: DaSubscription(updateRate, parentSession)
	{
		m_badItems = (ULONGLONG*)malloc(sizeof(ULONGLONG) * g_maxItemsPerSubscription);
		m_goodItems = (ULONGLONG*)malloc(sizeof(ULONGLONG) * g_maxItemsPerSubscription);
		m_badItemsCount = (DWORD*)malloc(sizeof(DWORD) * g_maxItemsPerSubscription);
		m_goodItemsCount = (DWORD*)malloc(sizeof(DWORD) * g_maxItemsPerSubscription);
		m_subNo = subNo;
	}

	unsigned int getMyDaSubscriptionItemsCount() const
	{
		return m_itemList.size();
	}

	void lock()
	{
		m_janitor.aLock();
	}

	void unlock()
	{
		m_janitor.aUnlock();
	}

	void handleStateChangeCompleted(EnumObjectState state)
	{
		if (g_tracesOn == true)
		{
			if (g_syncDisplay.aLock(0))
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
				g_syncDisplay.aUnlock();
			}
		}
		else if (m_subNo % g_silentDisplayCount == 0)
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T("sub%dStChCompl "), m_subNo);
				g_syncDisplay.aUnlock();
			}
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
				if (g_syncDisplay.aLock(0))
				{
					_tprintf(_T("\n"));
					_tprintf(_T("Subscription %d performed state transition - context: %lu\n"), m_subNo, executionContext);
					g_syncDisplay.aUnlock();
				} //end if
			}
			else if ((result != S_OK) || (m_subNo % g_silentDisplayCount == 0))
			{
				if (g_syncDisplay.aLock(0))
				{
					_tprintf(_T("sub%dProgStTrCompl(%d) "), m_subNo, result);
					g_syncDisplay.aUnlock();
				}
			}
		}
		else
		{
			if (g_syncDisplay.aLock(0))
			{
				_tprintf(_T("Subscription %d performed state transition failed - [0x%8.8X]"), m_subNo, result);
				_tprintf(_T("sub%dProgStTrCompl(%d) "), m_subNo, result);
				g_syncDisplay.aUnlock();
			}
		}
	} //handlePerformStateTransitionCompleted


	void handleDataChanged(
		const std::vector<DaItem*>& items,
		const std::vector<ValueQT*>& values,
		const std::vector<long>& results)
	{
		memset(m_badItems, 0, sizeof(ULONGLONG) * g_maxItemsPerSubscription);
		memset(m_goodItems, 0, sizeof(ULONGLONG) * g_maxItemsPerSubscription);
		memset(m_badItemsCount, 0, sizeof(DWORD) * g_maxItemsPerSubscription);
		memset(m_goodItemsCount, 0, sizeof(DWORD) * g_maxItemsPerSubscription);

		++g_dataChangeSetsReceived;
		unsigned long i;
		size_t count = items.size();

		int place;
		for (i = 0; i < count; i++)
		{
			if (!SUCCEEDED(results[i]))
			{
				int asdf = 123;
			}

			place = ((MyDaItem*)items[i])->getItemNo() % g_maxItemsPerSubscription;
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
		for (i = 0; i < g_maxItemsPerSubscription; ++i)
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

		if (g_syncDisplay.aLock(0))
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
			g_syncDisplay.aUnlock();
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
				if (g_syncDisplay.aLock(0))
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
					g_syncDisplay.aUnlock();
				}
			}
			else if ((result != S_OK) || (m_subNo % g_silentDisplayCount == 0))
			{
				if (g_syncDisplay.aLock(0))
				{
					_tprintf(_T("sub%dRd%dItmCompl(%d) "), m_subNo, items.size(), result);
					g_syncDisplay.aUnlock();
				}
			}
		}
		else
		{
			if (g_syncDisplay.aLock(0))
			{
				if (g_tracesOn == true)
				{
					_tprintf(_T("Read %d items failed - [0x%8.8X]\n"), items.size(), (unsigned int)result);
				}
				else
				{
					_tprintf(_T("sub%dRd%dItmCompl(%d) "), m_subNo, items.size(), result);
				}
				g_syncDisplay.aUnlock();
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
				if (g_syncDisplay.aLock(0))
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
					g_syncDisplay.aUnlock();
				}
			}
			else if ((result != S_OK) || (m_subNo % g_silentDisplayCount == 0))
			{
				if (g_syncDisplay.aLock(0))
				{
					_tprintf(_T("sub%dWr%dItmCompl(%d) "), m_subNo, items.size(), result);
					g_syncDisplay.aUnlock();
				}
			}
		}
		else
		{
			if (g_syncDisplay.aLock(0))
			{
				if (g_tracesOn == true)
				{
					_tprintf(_T("Write %d items failed - [0x%8.8X]\n"), items.size(), (unsigned int)result);
				}
				else
				{
					_tprintf(_T("sub%dWr%dItmCompl(%d) "), m_subNo, items.size(), result);
				}
				g_syncDisplay.aUnlock();
			}
		}//end if...else
	} //end handleWriteCompleted

	unsigned int removeDaItems(unsigned int count, std::vector<MyDaItem*>& items)
	{
		std::vector<DaItem*> currentItems = getItemList();
		DaItem* itemToRemove;
		unsigned int itemNo;
		unsigned int removedCount = 0;

		while (currentItems.size() > 0)
		{
			if ((getMyDaSubscriptionItemsCount() == 0) || (removedCount >= count))
			{
				break;
			}

			itemNo = rand() % currentItems.size();
			itemToRemove = currentItems[itemNo];

			if (itemToRemove == NULL)
			{
				DebugBreak();
			}

			if (g_tracesOn == true)
			{
				if (g_syncDisplay.aLock(0))
				{
					_tprintf(_T("\t\"%s\"\n"), itemToRemove->getId().c_str());
					g_syncDisplay.aUnlock();
				}
			}

			removeDaItem(itemToRemove);
			items.push_back((MyDaItem*)itemToRemove);
			currentItems = getItemList();
			++removedCount;
		}

		return count;
	}

	unsigned int addDaItems(const unsigned int count, std::vector<MyDaItem*>& items)
	{
		std::vector<MyDaItem*>::iterator itemToAddIt;
		MyDaItem* itemToAdd = NULL;
		MyDaItem* addedItem = NULL;
		std::vector<DaItem*> subItems;
		unsigned int addedCount = 0;
		unsigned int itemNo;

		while (items.size() > 0)
		{
			if ((getMyDaSubscriptionItemsCount() >= g_maxItemsPerSubscription) || (addedCount >= count))
			{
				break;
			}

			itemNo = rand() % items.size();
			itemToAddIt = items.begin();
			std::advance(itemToAddIt, itemNo);
			itemToAdd = *itemToAddIt;
			if (itemToAdd == NULL)
			{
				DebugBreak();
			}
			addedItem = new MyDaItem(itemToAdd->getItemNo(), itemToAdd->getMyDaItemId(), this, itemToAdd->getDataType());

			if (g_tracesOn == true)
			{
				if (g_syncDisplay.aLock(0))
				{
					_tprintf(_T("\t\"%s\"\n"), addedItem->getMyDaItemId().c_str());
					g_syncDisplay.aUnlock();
				}
			}

			if ((g_connectItemsFirst) && (getCurrentState() != EnumObjectState_DISCONNECTED))
			{
				addedItem->connect(FALSE, getCurrentState() == EnumObjectState_ACTIVATED, NULL);
			}

			items.erase(itemToAddIt);
			delete itemToAdd;
			++addedCount;
		}

		return addedCount;
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
	} // end dtor

};  // end class MyDaSubscription

#endif
