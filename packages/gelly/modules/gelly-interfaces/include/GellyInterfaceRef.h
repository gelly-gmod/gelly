#ifndef GELLYINTERFACEREF_H
#define GELLYINTERFACEREF_H

#include <memory>

#include "GellyObserverPtr.h"

template <typename T>
using GellyInterfaceCRef = const GellyObserverPtr<T> &;

template <typename T>
using GellyInterfaceRef = GellyObserverPtr<T> &;

template <typename T>
using GellyInterfaceVal = GellyObserverPtr<T>;

// this is great for implementing RAII-like semantics while still keeping the
// interfaces
template <typename T>
using GellyOwnedInterface = std::unique_ptr<T>;

#endif	// GELLYINTERFACEREF_H
