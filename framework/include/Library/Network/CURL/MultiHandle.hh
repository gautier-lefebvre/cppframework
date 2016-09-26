#ifndef   __LIBRARY_NETWORK_CURL_MULTIHANDLE_HH__
#define   __LIBRARY_NETWORK_CURL_MULTIHANDLE_HH__

#include  <vector>
#include  <curl/curl.h>

#include  "Library/Network/CURL/EasyHandle.hh"

namespace fwk {
    namespace curlxx {
        /**
         *  \class MultiHandle Library/Network/CURL/MultiHandle.hh
         *  \brief Encapsulation of the curl multi handle (see curl man).
         */
        class MultiHandle {
        public:
            static const size_t PIPELINED_REQUESTS_MAX_NB; /*!< max number of requests sent at once (arbitrary, should be small (~10) not to ddos the remote server by mistake). */

        private:
            CURLM* _multiHandle; /*!< the underlying curl multi handle. */
            std::vector<curlxx::EasyHandle*> _easyHandles; /*!< the easy handles used by this multi handle. */

        public:
            /**
             *  \brief Constructor of MultiHandle.
             */
            MultiHandle(void);

            /**
             *  \brief Destructor of MultiHandle.
             */
            virtual ~MultiHandle(void);

        public:
            /**
             *  \brief Finds an EasyHandle given a curl easy handle object.
             *  \throw curlxx::Exception if the curl easy handle object is nullptr.
             *  \param curlHandle the curl easy handle object.
             *  \return the found EasyHandle, or nullptr if not found.
             */
            EasyHandle*  findHandle(CURL* curlHandle);

            /**
             *  \brief Returns true if the multi handle has easy handles.
             *  \return true if the multi handle has easy handles;
             */
            bool  hasHandles(void) const;

        public:
            /**
             *  \brief Encapsulation of the curl_multi_init function.
             *  \throw curlx::Exception if the underlying function failed.
             */
            void  init(void);

            /**
             *  \brief Encapsulation of the curl_multi_add_handle function.
             *  \throw curlx::Exception if the underlying function failed, or if the maximum number of handles was reached.
             *  \param handle the easy handle to add.
             */
            void  addHandle(EasyHandle* handle);

            /**
             *  \brief Encapsulation of the curl_multi_remove_handle function.
             *  \throw curlx::Exception if the underlying function failed, or if the given handle was nullptr.
             *  \param handle the easy handle to remove. If it isn't in the MultiHandle, does nothing.
             */
            void  removeHandle(EasyHandle* handle);

            /**
             *  \brief Encapsulation of the curl_multi_perform function.
             *  \throw curlx::Exception if the underlying function failed.
             *  \return true if the method must be called again (the operation is not done).
             */
            bool  perform(void) const;

            /**
             *  \brief Encapsulation of the curl_multi_timeout function.
             *  \throw curlx::Exception if the underlying function failed.
             *  \param interval a timeval structure which will be filled with the recommended duration of timeout for select.
             */
            void  timeout(struct timeval& interval) const;

            /**
             *  \brief Encapsulation of the curl_multi_fdset function.
             *  If the fdmax is -1 after this function, sleep instead of calling select.
             *  \throw curlx::Exception if the underlying function failed.
             *  \param rset empty read fd_set, or NULL.
             *  \param wset empty write fd_set, or NULL.
             *  \param eset empty exception fd_set, or NULL.
             *  \param fdmax an int which will be set to the maximum fd, or -1 if no socket is yet ready.
             */
            void  fdset(fd_set* rset, fd_set* wset, fd_set* eset, int& fdmax) const;

            /**
             *  \brief Encapsulation of the curl_multi_info_read function.
             *  \return the next CURLMsg in the queue, or NULL (see return value of the curl_multi_info_read function).
             */
            CURLMsg*  infoRead(void) const;

            /**
             *  \brief Cleans up the multi handle and all easy handles.
             */
            void  cleanup(void);
        };
    }
}

#endif    /* __LIBRARY_NETWORK_CURL_MULTIHANDLE_HH__ */
