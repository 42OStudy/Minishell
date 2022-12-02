/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jiyunpar <jiyunpar@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/12/02 10:39:03 by jiyunpar          #+#    #+#             */
/*   Updated: 2022/12/02 11:20:41y ccjiyunpar       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool is_logical_and(char *cmd)
{
	if (ft_strcmp(cmd, "&&") == 0)
		return (true);
	return (false);
}

bool is_logical_or(char *cmd)
{
	if (ft_strcmp(cmd, "||") == 0)
		return (true);
	return (false);
}

void	skip_node_to_logical_operator(t_node **cur_node)
{
	t_field	*cur_field;
	t_token	*start_token;

	while (*cur_node)
	{	
		cur_field = (t_field *)((*cur_node)->content);
		start_token = (t_token *)(cur_field->start_ptr->content);
		if (is_logical_and(start_token->value)
			|| is_logical_or(start_token->value))
			break ;
		(*cur_node) = (*cur_node)->next;
	}
}

void	execute(t_list *exec_list)
{
	t_node	*cur_node;
	t_field	*cur_field;
	t_token	*start_token;

	cur_node = exec_list->head;
	while (cur_node)
	{
		cur_field = (t_field *)(cur_node->content);
		start_token = (t_token *)(cur_field->start_ptr->content);
		if (is_logical_and(start_token->value))
		{	
			if (g_exit_status == 0)
				cur_node = cur_node->next;
			else
				skip_node_to_logical_operator(&cur_node);
		}
		else if (is_logical_or(start_token->value))
		{
			if (g_exit_status == 0)
				skip_node_to_logical_operator(&cur_node);
			else
				cur_node = cur_node->next;
		}
		else if (ft_strcmp(start_token->value, "|"))
		{
			cur_node = cur_node->next;
		}
		else
		{
			int		fd_pipe[2];
			pid_t	pid;
			int		cur_pipe_in;
			int		cur_pipe_out;
			int		prev_pipe_in = -1;
			char	**command;

			if (cur_node->next == "|")
			{
				if (pipe(fd_pipe) == -1)
					ft_terminate("execute, pipe");
				cur_pipe_in = fd_pipe[0];
				cur_pipe_out = fd_pipe[1];
				// fork뜨기 직전에 확장한다?
				pid = fork();
				if (pid == -1)
					ft_terminate("execute, fork");
				if (pid == 0)
				{
					if (prev_pipe_in != -1) // 이전 파이프가 있었으면
					{
						dup2(prev_pipe_in, 0);
						close(prev_pipe_in);
					}
					dup2(cur_pipe_out, 1);
					close(cur_pipe_out);
					close(cur_pipe_in);
					expand_field(cur_field);
					redirectioin();
					command = refine_field(cur_field);
					execve("/bin/echo", command, envp); // ./minishell, PATH 
					// export a=*.c;
					// cur_pipe_in을 prev_pipe_in에 담아주면 닫기.
				}
				// dup 
				// expand
				// redirection
				// argv parsing
				// execute
			}
			else
			{
				pid = fork();
				if (pid == -1)
					ft_terminate("execute, fork");
				// expand
				// redirection
				// argv parsing
				// execute
			{

			}
			cur_node = cur_node->next;
		}
	}
	// wait_pid
}
	/*
		리스트 순회 - X
		현재 리스트와 다음 리스트를 비교하면서 연산자마다 행동 지정
		현재 노드의 content가 && ,||, |
		오퍼레이터가 아니라면 다음 노드가 파이프 인지 확인
		- 다음 필드가 파이프인지  본다.
			- 파이프를 열여주고 포크하기위해서
		- 지금 필드가 ||, && 라면 
			- exit_status확인해서 
				- &&
					- EXIT_STATUS 가 0이라면 실핸한다.
					- EXIT_STATUS 가 0이아니면 다음 node는 건너뜀 
				- ||
					- EXIT_STATUS 가 0이라면 실행 안하고 다음 node 건너뜀.
					- EXIT_STATUS 가 0이아니면 다음 node 실행 
		- 지금 필드에 리다이렉션(<, >, <<, >>)이 있다면
				- fork()하고 자식에서 이를 처리한다.
		- 지금 필드에 리다이렉션이 있고, 다음 필드에 파이프라면
				- 파이프 셋팅해주고
				- 자식에서 리다이렉션 해주면 된다.                                                                                                                                                                                                                                              
	*/

	/*
		부모
		1. 파이프가 있으면 파이프를 열고
		2. 포크
			자식
			1. 파이프가 열려있으면 파이프셋팅(표준 입출력 셋팅 파이프를 가르키도록)
			2. 확장
			3. 리다이렉션
			4. 정제된 필드 만들기
			5. execv 함수로 넘긴다
		3. 부모는 자식이 쓰고 있는 파이프 제외하고 다 닫아준다. (+ prev_pipe_in)
		4. 다음 필드로 간다.
			- 다음 필드가 ||, &&이라면
	*/
