#ifndef GELLYINTERFACEREF_H
#define GELLYINTERFACEREF_H

#include "GellyObserverPtr.h"

template <typename T>
using GellyInterfaceCRef = const GellyObserverPtr<T> &;

template <typename T>
using GellyInterfaceRef = GellyObserverPtr<T> &;

#endif	// GELLYINTERFACEREF_H
