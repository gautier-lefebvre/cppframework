#ifndef		__CORE_WORKER_TASK_HH__
#define		__CORE_WORKER_TASK_HH__

#include	<chrono>

#include	"Core/Event/EventBase.hh"
#include	"Core/Event/IEventArgs.hh"

namespace		Core {
	namespace	Worker {
		class	ATask :public Factory::AFactored {
		public:
			enum class Source {
				EVENT,
				HTTP_CALLBACK,
				PERIODIC_TASK
			};

		private:
			Source	_source;

		public:
			ATask(Source);
			virtual ~ATask(void);

		public:
			virtual void reinit(void) = 0;

		public:
			Source	getSource(void) const;
		};

		class	EventTask :public ATask {
		public:
			const Core::Event::EventBase*	_event;
			Core::Event::IEventArgs*		_args;

		public:
			EventTask(void);
			virtual ~EventTask(void);

		public:
			virtual void reinit(void);

		public:
			void init(const Core::Event::EventBase*, Core::Event::IEventArgs*);

		public:
			struct	Pool :public Singleton<Core::Worker::EventTask::Pool>, public Factory::BasicPool<Core::Worker::EventTask> {
				friend class Singleton<Core::Worker::EventTask::Pool>;
			public:
				const size_t	ORIGINAL_SIZE = 100;
				const size_t	HYDRATE_SIZE = 20;

			private:
				Pool(const Pool&) = delete;
				Pool(const Pool&&) = delete;
				Pool& operator=(const Pool&) = delete;

			private:
				Pool(void);
				virtual ~Pool(void);

			public:
				void init(void);
			};
		};

		class	HTTPTask :public ATask {
		public:
			// set http args

		public:
			EventTask(void);
			virtual ~EventTask(void);

		public:
			virtual void reinit(void);

		public:
			void init(void); // same

		public:
			struct	Pool :public Singleton<Core::Worker::HTTPTask::Pool>, public Factory::BasicPool<Core::Worker::HTTPTask> {
				friend class Singleton<Core::Worker::HTTPTask::Pool>;
			public:
				const size_t	ORIGINAL_SIZE = 100;
				const size_t	HYDRATE_SIZE = 20;

			private:
				Pool(const Pool&) = delete;
				Pool(const Pool&&) = delete;
				Pool& operator=(const Pool&) = delete;

			private:
				Pool(void);
				virtual ~Pool(void);

			public:
				void init(void);
			};
		};

		class	PeriodicTask :public ATask {
		public:
			std::function<void (void)>	_callback;
			std::function<void (void)>	_clean;
			std::chrono::steady_clock::duration	_interval;
			bool	_off;

		public:
			PeriodicTask(void);
			virtual ~PeriodicTask(void);

		public:
			virtual void reinit(void);

		public:
			void	init(const std::function<void(void)>&, const std::function<void(void)>&, const std::chrono::steady_clock::duration&);
			void	stop(bool);

		public:
			struct	Pool :public Singleton<Core::Worker::PeriodicTask::Pool>, public Factory::BasicPool<Core::Worker::PeriodicTask> {
				friend class Singleton<Core::Worker::PeriodicTask::Pool>;
			public:
				const size_t	ORIGINAL_SIZE = 20;
				const size_t	HYDRATE_SIZE = 10;

			private:
				Pool(const Pool&) = delete;
				Pool(const Pool&&) = delete;
				Pool& operator=(const Pool&) = delete;

			private:
				Pool(void);
				virtual ~Pool(void);

			public:
				void init(void);
			};
		};

		class	DelayedTask :public Factory::AFactored {
		public:
			ATask*	_task;
			std::chrono::steady_clock::time_point	_timePoint;

		public:
			DelayedTask(void);
			virtual ~DelayedTask(void);

		public:
			virtual void reinit(void);

		public:
			void	init(ATask*, const std::chrono::steady_clock::time_point&);
			void	init(ATask*, const std::chrono::steady_clock::duration&);

		public:
			bool operator<(const DelayedTask&) const;
			bool operator>(const DelayedTask&) const;
			bool operator==(const ATask*) const;

		public:
			struct	Pool :public Singleton<Core::Worker::DelayedTask::Pool>, public Factory::BasicPool<Core::Worker::DelayedTask> {
				friend class Singleton<Core::Worker::DelayedTask::Pool>;
			public:
				const size_t	ORIGINAL_SIZE = 100;
				const size_t	HYDRATE_SIZE = 20;

			private:
				Pool(const Pool&) = delete;
				Pool(const Pool&&) = delete;
				Pool& operator=(const Pool&) = delete;

			private:
				Pool(void);
				virtual ~Pool(void);

			public:
				void init(void);
			};
		};
	}
}

#endif		/* __CORE_WORKER_TASK_HH__ */
