#include  "Library/Network/CURL/MultiHandle.hh"
#include  "Library/Network/CURL/Exception.hh"
#include  "Library/Tool/Logger.hpp"

using namespace fwk;

const size_t curlxx::MultiHandle::PIPELINED_REQUESTS_MAX_NB = 10;

curlxx::MultiHandle::MultiHandle(void):
    _multiHandle(NULL),
    _easyHandles(curlxx::MultiHandle::PIPELINED_REQUESTS_MAX_NB, nullptr)
{}

curlxx::MultiHandle::~MultiHandle(void) {
    this->cleanup();
}

void curlxx::MultiHandle::init(void) {
    if ((this->_multiHandle = curl_multi_init()) == NULL) {
        throw curlxx::Exception("curl_multi_init failed");
    }
}

void curlxx::MultiHandle::cleanup(void) {
    // return all easy handles to pool
    for (size_t i = 0 ; i < curlxx::MultiHandle::PIPELINED_REQUESTS_MAX_NB ; ++i) {
        curlxx::EasyHandle::returnToPool(this->_easyHandles[i]);
        this->_easyHandles[i] = nullptr;
    }

    if (this->_multiHandle != NULL) {
        curl_multi_cleanup(this->_multiHandle);
    }

    this->_multiHandle = NULL;
}

void curlxx::MultiHandle::addHandle(EasyHandle* handle) {
    // add handle to the first empty cell
    for (size_t i = 0 ; i < curlxx::MultiHandle::PIPELINED_REQUESTS_MAX_NB ; ++i) {
        if (this->_easyHandles[i] == nullptr) {
            this->_easyHandles[i] = handle;
            curl_multi_add_handle(this->_multiHandle, handle->getHandle());
            return;
        }
    }

    // throw exception if no available cell
    throw curlxx::Exception("addHandle: too many handles already attached to this pipeline");
}

void curlxx::MultiHandle::removeHandle(EasyHandle* handle) {
    if (handle == nullptr) {
        throw curlxx::Exception("curl_multi_remove_handle: trying to remove a null pointer");
    }

    for (size_t i = 0 ; i < curlxx::MultiHandle::PIPELINED_REQUESTS_MAX_NB ; ++i) {
        if (this->_easyHandles[i] == handle) {
            curl_multi_remove_handle(this->_multiHandle, handle);
            curlxx::EasyHandle::returnToPool(this->_easyHandles[i]);
            this->_easyHandles[i] = nullptr;
            break;
        }
    }
}

curlxx::EasyHandle* curlxx::MultiHandle::findHandle(CURL* handle) {
    if (handle == nullptr) {
        throw curlxx::Exception("Trying to find a null pointer");
    }

    for (size_t i = 0 ; i < curlxx::MultiHandle::PIPELINED_REQUESTS_MAX_NB ; ++i) {
        if (this->_easyHandles[i] != nullptr && this->_easyHandles[i]->getHandle() == handle) {
            return this->_easyHandles[i];
        }
    }

    return nullptr;
}

bool curlxx::MultiHandle::perform(void) const {
    int still_running;
    CURLMcode ret;

    ret = curl_multi_perform(this->_multiHandle, &still_running);

    if (ret == CURLM_CALL_MULTI_PERFORM) {
        // curl_multi wants to be called again
        return this->perform();
    } else if (ret != CURLM_OK) {
        // an error occurred
        throw curlxx::Exception(fmt::format("curl_multi_perform failed: {0}", curl_multi_strerror(ret)));
    }

    return (still_running != 0);
}

void curlxx::MultiHandle::timeout(struct timeval& to) const {
    long curl_timeout;
    CURLMcode ret;

    if ((ret = curl_multi_timeout(this->_multiHandle, &curl_timeout))) {
        throw curlxx::Exception(fmt::format("curl_multi_perform failed: {0}", curl_multi_strerror(ret)));
    }

    if (curl_timeout < 0) {
        curl_timeout = 1000;
    }

    to.tv_sec = curl_timeout / 1000;
    to.tv_usec = (curl_timeout % 1000) * 1000;
}

void curlxx::MultiHandle::fdset(fd_set* rset, fd_set* wset, fd_set* eset, int& maxfd) const {
    CURLMcode ret;

    if (rset) {
        FD_ZERO(rset);
    }

    if (wset) {
        FD_ZERO(wset);
    }

    if (eset) {
        FD_ZERO(eset);
    }

    if ((ret = curl_multi_fdset(this->_multiHandle, rset, wset, eset, &maxfd)) != CURLM_OK) {
        throw curlxx::Exception(fmt::format("curl_multi_fdset failed: {0}", curl_multi_strerror(ret)));
    }
}

CURLMsg* curlxx::MultiHandle::infoRead(void) const {
    int msgs_left;
    return curl_multi_info_read(this->_multiHandle, &msgs_left);
}

bool curlxx::MultiHandle::hasHandles(void) const {
    for (size_t i = 0 ; i < curlxx::MultiHandle::PIPELINED_REQUESTS_MAX_NB ; ++i) {
        if (this->_easyHandles[i] != nullptr) {
            return true;
        }
    }

    return false;
}
