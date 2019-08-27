﻿#pragma once
#include<cstdlib>

namespace TA {
	class mempool {
	private:
		enum { UNIT = 8 };
		enum { MAXSIZE = 128 };
		enum { LISTLEN = MAXSIZE / UNIT };
		union listnode {
			union listnode* next;
			char memaddr[1];
		};
		static listnode* volatile listheadarr[LISTLEN];
		static  char* end_free;
		static  char* start_free;
		static  size_t heap_size;

		/*align the user's memunit size*/
		static size_t round_up(size_t bytes) {
			return (bytes + UNIT - 1) & ~(UNIT - 1);//!!!!!
		}
		/*to get the position fo the mem block of this size*/
		static size_t getlistindex(size_t bytes) {
			return (bytes + UNIT - 1) / UNIT - 1;
		}

		static void* refill(size_t n) {
			size_t nodenum = 20;
			char* chunk = chunkalloc(n, nodenum);
			if (nodenum == 1) {
				return chunk;
			}

			listnode* volatile* listhead;
			listnode* current_node;
			listnode* next_node;


			listhead = listheadarr + getlistindex(n);
			*listhead = next_node = (listnode*)(chunk + n);

			//TODO:this loop needs to be simplyfy
			//for (int i = 1;; i++) {
			//	current_node = next_node;
			//	next_node = (listnode*)((char*)next_node + n);
			//	if (nodenum - 1 == i) {
			//		current_node->next = 0;
			//		break;
			//	} else {
			//		current_node->next = next_node;
			//	}
			//}

			for (int i = 1; nodenum - 1 != i; i++) {
				current_node = next_node;
				next_node = (listnode*)((char*)next_node + n);
				current_node->next = next_node;
			}
			current_node = next_node;
			next_node = (listnode*)((char*)next_node + n);
			current_node->next = 0;

			return (listnode*)chunk;
		}

		static char* chunkalloc(size_t nodesize, size_t& nodenum) {

			size_t total_bytes = nodesize * nodenum;
			size_t bytes_left = end_free - start_free;
			char* result;

			//TODO:too complicated
			if (bytes_left >= total_bytes) {
				result = start_free;
				start_free += total_bytes;
				return result;
			} else if (bytes_left >= nodesize) {
				nodenum = bytes_left / nodesize;
				total_bytes = nodesize * nodenum;

				result = start_free;
				start_free += total_bytes;
				return result;//!!!!!
			} else {
				//将剩余的一点内存分配到最合适的地方去
				size_t bytes_to_get = 2 * total_bytes + round_up(heap_size >> 4);//???

				if (bytes_left > 0) {
					listnode* volatile* listhead = listheadarr + getlistindex(bytes_left);
					((listnode*)start_free)->next = *listhead;
					*listhead = (listnode*)start_free;
				}

				//从系统申请新的内存
				start_free = (char*)malloc(bytes_to_get);
				end_free = start_free + bytes_to_get;
				heap_size += bytes_to_get;

				return chunkalloc(nodesize, nodenum);
			}
		}



	public:
		static void* allocate(size_t n) {
			listnode* volatile* listhead;
			listnode* result;

			if (n > (size_t)MAXSIZE) {
				return malloc(n);
			}
			listhead = listheadarr + getlistindex(n);
			result = *listhead;
			if (result == 0) {//? where the 0 come from?
				return refill(round_up(n));
			}
			*listhead = result->next;
			return result;
		}

		static void deallocate(void* p, size_t n) {
			listnode* q = (listnode*)p;
			listnode* volatile* listhead;

			if (n > (size_t)MAXSIZE) {
				free(p);//为了不与本类中的free冲突
				return;

			}

			listhead = listheadarr + getlistindex(n);

			q->next = *listhead;
			*listhead = q;
		}
	};

	mempool::listnode* volatile mempool::listheadarr[LISTLEN] = { 0 };
	char* mempool::end_free = 0;
	char* mempool::start_free = 0;
	size_t mempool::heap_size = 0;
}