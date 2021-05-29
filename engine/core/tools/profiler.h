//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#ifndef _REV_CORE_TOOLS_PROFILER_H_
#define _REV_CORE_TOOLS_PROFILER_H_

#include <fstream>
#include <vector>

namespace rev {
	namespace core {

		class Profiler {
		public:
			Profiler(){
				records.reserve(100);
				std::ofstream log("profiler.txt"); // Clear the log
			}

			~Profiler() {
				flush();
			}

			void update(float _dt) {
				records.push_back(_dt);
				if (records.size() == records.capacity()) {
					flush();
				}
			}

		private:
			void flush() {
				std::ofstream log("profiler.txt", std::ios::app);
				for (auto r : records)
					log << r << "\n";
				log.close();
				records.clear();
			}

			std::vector<float>	records;
		};

} }

#endif // _REV_CORE_TOOLS_PROFILER_H_