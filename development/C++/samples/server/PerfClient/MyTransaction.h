#ifndef _MYTRANSACTION_H_
#define _MYTRANSACTION_H_

#include "Da/ServerDaTransaction.h"
#include "MyDaAddressSpaceElement.h"

using namespace SoftingOPCToolboxServer;

class MyTransaction : public DaTransaction
{

public:

	MyTransaction(
		EnumTransactionType aTransactionType,
		std::vector<DaRequest*>& aRequestList,
		unsigned long aSessionKey) :
		DaTransaction(
			aTransactionType,
			aRequestList,
			aSessionKey)
	{} // end ctor

	virtual ~MyTransaction()
	{} // end dtor

	virtual long handleReadRequests(void)
	{
		size_t count = m_requestList.size();

		for (size_t i = 0; i < count; i++)
		{
			MyDaAddressSpaceElement* element =
				(MyDaAddressSpaceElement*)m_requestList[i]->getAddressSpaceElement();

			if (element == NULL)
			{
				m_requestList[i]->setResult(E_FAIL);
			}
			else
			{
				if (m_requestList[i]->getPropertyId() == 0)
				{
					if (element->getIsChanging())
					{
						ValueQT value;
						DateTime now;
						now.now();

						switch (element->getDatatype())
						{
						case VT_I1:
						{
							element->getCacheValue(value);
							value.getData().cVal++;
							ValueQT val(value.getData(), EnumQuality_GOOD, now);
							element->valueChanged(val);
						}
						break;

						case VT_UI1:
						{
							element->getCacheValue(value);
							value.getData().bVal++;
							ValueQT val(value.getData(), EnumQuality_GOOD, now);
							element->valueChanged(val);
						}
						break;

						case VT_I2:
						{
							element->getCacheValue(value);
							value.getData().iVal++;
							ValueQT val(value.getData(), EnumQuality_GOOD, now);
							element->valueChanged(val);
						}
						break;

						case VT_UI2:
						{
							element->getCacheValue(value);
							value.getData().uiVal++;
							ValueQT val(value.getData(), EnumQuality_GOOD, now);
							element->valueChanged(val);
						}
						break;

						case VT_I4:
						{
							element->getCacheValue(value);
							value.getData().lVal++;
							ValueQT val(value.getData(), EnumQuality_GOOD, now);
							element->valueChanged(val);
						}
						break;

						case VT_UI4:
						{
							element->getCacheValue(value);
							value.getData().ulVal++;
							ValueQT val(value.getData(), EnumQuality_GOOD, now);
							element->valueChanged(val);
						}
						break;

						case VT_INT:
						{
							element->getCacheValue(value);
							value.getData().intVal++;
							ValueQT val(value.getData(), EnumQuality_GOOD, now);
							element->valueChanged(val);
						}
						break;

						case VT_UINT:
						{
							element->getCacheValue(value);
							value.getData().uintVal++;
							ValueQT val(value.getData(), EnumQuality_GOOD, now);
							element->valueChanged(val);
						}
						break;

						case VT_I8:
						{
							element->getCacheValue(value);
							value.getData().llVal++;
							ValueQT val(value.getData(), EnumQuality_GOOD, now);
							element->valueChanged(val);
						}
						break;

						case VT_UI8:
						{
							element->getCacheValue(value);
							value.getData().ullVal++;
							ValueQT val(value.getData(), EnumQuality_GOOD, now);
							element->valueChanged(val);
						}
						break;

						case VT_R4:
						{
							element->getCacheValue(value);
							value.getData().fltVal++;
							ValueQT val(value.getData(), EnumQuality_GOOD, now);
							element->valueChanged(val);
						}
						break;

						case VT_R8:
						{
							element->getCacheValue(value);
							value.getData().dblVal++;
							ValueQT val(value.getData(), EnumQuality_GOOD, now);
							element->valueChanged(val);
						}
						break;

						case VT_BOOL:
						{
							element->getCacheValue(value);
							value.getData().boolVal = !(value.getData().boolVal);
							ValueQT val(value.getData(), EnumQuality_GOOD, now);
							element->valueChanged(val);
						}
						break;

						case VT_BSTR:
						{
							element->getCacheValue(value);
							BSTR w = (value.getData().bstrVal);
#ifdef TBC_OS_WINDOWS

							if (w)
							{
								while (*w != 0)
								{
									(*w)++;

									if (*w == OLESTR('Z') + 1)
									{
										*w = OLESTR('A');
									}

									if (*w == OLESTR('z') + 1)
									{
										*w = OLESTR('a');
									}

									if (*w == OLESTR('9') + 1)
									{
										*w = OLESTR('0');
									}

									if (!(((*w >= OLESTR('A')) && (*w <= OLESTR('Z'))) ||
										  ((*w >= OLESTR('a')) && (*w <= OLESTR('z'))) ||
										  ((*w >= OLESTR('0')) && (*w <= OLESTR('9')))))
									{
										*w = L' ';
									}

									w++;
								}
							}

#endif
#ifdef TBC_OS_LINUX

							if (w)
							{
								while (*w != 0)
								{
									(*w)++;

									if (*w == ('Z') + 1)
									{
										*w = ('A');
									}

									if (*w == ('z') + 1)
									{
										*w = ('a');
									}

									if (*w == ('9') + 1)
									{
										*w = ('0');
									}

									if (!(((*w >= ('A')) && (*w <= ('Z'))) ||
										  ((*w >= ('a')) && (*w <= ('z'))) ||
										  ((*w >= ('0')) && (*w <= ('9')))))
									{
										*w = _TCHAR(' ');
									}

									w++;
								}
							}

#endif
							ValueQT val(value.getData(), EnumQuality_GOOD, now);
							element->valueChanged(val);
						}
						break;

						case VT_DATE:
						{
							element->getCacheValue(value);
							DATE* pDate = &(value.getData().date);

							if (pDate)
							{
#ifdef TBC_OS_WINDOWS
								*pDate += 1.0 / (24 * 3600);
#else // TBC_OS_WINDOWS
								++pDate->tv_sec;
#endif
							}

							value.getData().date = *pDate;
							ValueQT val(value.getData(), EnumQuality_GOOD, now);
							element->valueChanged(val);
						}
						break;

						case VT_CY:
						{
							element->getCacheValue(value);
							value.getData().cyVal;
							ValueQT val(value.getData(), EnumQuality_GOOD, now);
							element->valueChanged(val);
						}
						break;

						case VT_VARIANT:
						{
							element->getCacheValue(value);
							ValueQT val(value.getData(), EnumQuality_GOOD, now);
							element->valueChanged(val);
						}
						break;

						case (VT_ARRAY | VT_I1):
						case (VT_ARRAY | VT_UI1):
						case (VT_ARRAY | VT_I2):
						case (VT_ARRAY | VT_UI2):
						case (VT_ARRAY | VT_I4):
						case (VT_ARRAY | VT_UI4):
						case (VT_ARRAY | VT_INT):
						case (VT_ARRAY | VT_UINT):
						case (VT_ARRAY | VT_R4):
						case (VT_ARRAY | VT_R8):
						case (VT_ARRAY | VT_BOOL):
						case (VT_ARRAY | VT_BSTR):
						case (VT_ARRAY | VT_DATE):
						case (VT_ARRAY | VT_CY):
						case (VT_ARRAY | VT_VARIANT):
						{
							element->getCacheValue(value);
							LONG lBound = 0;
							LONG uBound = 0;
							LONG idx;
							void* voidArray = 0;
							Variant::SafeArrayGetLBound(value.getData().parray, 1, &lBound);
							Variant::SafeArrayGetUBound(value.getData().parray, 1, &uBound);
							Variant::SafeArrayAccessData(value.getData().parray, (void**)&voidArray);

							for (idx = lBound; idx <= uBound; idx++)
							{
								VARENUM type = (VARENUM)(value.getData().vt & ~VT_ARRAY);

								switch (type)
								{
								case VT_I1:
								{
									CHAR* cVal = ((CHAR*)voidArray) + idx;
									(*cVal)++;
								}
								break;

								case VT_UI1:
								{
									BYTE* bVal = ((BYTE*)voidArray) + idx;
									(*bVal)++;
								}
								break;

								case VT_I2:
								{
									SHORT* iVal = ((SHORT*)voidArray) + idx;
									(*iVal)++;
								}
								break;

								case VT_UI2:
								{
									USHORT* uiVal = ((USHORT*)voidArray) + idx;
									(*uiVal)++;
								}
								break;

								case VT_I4:
								{
									LONG* lVal = ((LONG*)voidArray) + idx;
									(*lVal)++;
								}
								break;

								case VT_UI4:
								{
									ULONG* ulVal = ((ULONG*)voidArray) + idx;
									(*ulVal)++;
								}
								break;

								case VT_INT:
								{
									int* iVal = ((int*)voidArray) + idx;
									(*iVal)++;
								}
								break;

								case VT_UINT:
								{
									unsigned int* uiVal = ((unsigned int*)voidArray) + idx;
									(*uiVal)++;
								}
								break;

								case VT_R4:
								{
									FLOAT* fltVal = ((FLOAT*)voidArray) + idx;
									(*fltVal)++;
								}
								break;

								case VT_R8:
								{
									DOUBLE* dblVal = ((DOUBLE*)voidArray) + idx;
									(*dblVal)++;
								}
								break;

								case VT_BOOL:
								{
									VARIANT_BOOL* boolVal = ((VARIANT_BOOL*)voidArray) + idx;
									(*boolVal) = !(*boolVal);
								}
								break;

								case VT_BSTR:
								{
									BSTR* bstrVal = ((BSTR*)voidArray) + idx;
									BSTR w = *bstrVal;
#ifdef TBC_OS_WINDOWS

									if (w)
									{
										while (*w != 0)
										{
											(*w)++;

											if (*w == OLESTR('Z') + 1)
											{
												*w = OLESTR('A');
											}

											if (*w == OLESTR('z') + 1)
											{
												*w = OLESTR('a');
											}

											if (*w == OLESTR('9') + 1)
											{
												*w = OLESTR('0');
											}

											if (!(((*w >= OLESTR('A')) && (*w <= OLESTR('Z'))) ||
												  ((*w >= OLESTR('a')) && (*w <= OLESTR('z'))) ||
												  ((*w >= OLESTR('0')) && (*w <= OLESTR('9')))))
											{
												*w = L' ';
											}

											w++;
										}
									}

#endif
#ifdef TBC_OS_LINUX

									if (w)
									{
										while (*w != 0)
										{
											(*w)++;

											if (*w == ('Z') + 1)
											{
												*w = ('A');
											}

											if (*w == ('z') + 1)
											{
												*w = ('a');
											}

											if (*w == ('9') + 1)
											{
												*w = ('0');
											}

											if (!(((*w >= ('A')) && (*w <= ('Z'))) ||
												  ((*w >= ('a')) && (*w <= ('z'))) ||
												  ((*w >= ('0')) && (*w <= ('9')))))
											{
												*w = _TCHAR(' ');
											}

											w++;
										}
									}

#endif
								}
								break;

								case VT_DATE:
								{
									DATE* dateVal = ((DATE*)voidArray) + idx;
									DATE* pDate = dateVal;

									if (pDate)
									{
#ifdef TBC_OS_WINDOWS
										*pDate += 1.0 / (24 * 3600);
#else // TBC_OS_WINDOWS
										++pDate->tv_sec;
#endif
									}
								}
								break;

								case VT_CY:
									//TODO
									// not used atm
									break;

								case VT_VARIANT:
									//TODO
									// not used atm
									break;

								default:
								{
									// nothing to do
								}
								break;
								}
							}

							Variant::SafeArrayUnaccessData(value.getData().parray);
							ValueQT aValueQT(value.getData(), EnumQuality_GOOD, now);
							element->valueChanged(aValueQT);
						}
						break;

						default:
						{
							//nothing to do
						}
						break;
						}// end switch
					}
					else
					{
						// get address space element value take the Toolkit cache value
						ValueQT cacheValue;
						element->getCacheValue(cacheValue);
						m_requestList[i]->setValue(cacheValue);
						m_requestList[i]->setResult(S_OK);
					}
				}
				else
				{
					// the element's property will handle this request
					element->getPropertyValue(m_requestList[i]);
				} // end if ... else
			} // end if ... else
		} // end for

		return completeRequests();
	} // end HandleReadRequests

	virtual long handleWriteRequests(void)
	{
		size_t count = m_requestList.size();

		for (size_t i = 0; i < count; i++)
		{
			DaRequest* pRequest = m_requestList[i];

			if (pRequest != NULL)
			{
				MyDaAddressSpaceElement* pElement =
					(MyDaAddressSpaceElement*)pRequest->getAddressSpaceElement();

				if (pElement != NULL)
				{
					ValueQT* pValue = pRequest->getValue();
					pRequest->setResult(pElement->valueChanged(*pValue));
				} // end if
			} // end if
		} // end  for

		return completeRequests();
	} // end handleWriteRequests

};  // end class MyTransaction

#endif  // _MYTRANSACTION_H_
