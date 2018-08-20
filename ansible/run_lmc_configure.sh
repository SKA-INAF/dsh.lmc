#!/bin/sh


ansible-playbook -i hosts localhost_server.yml --limit localhost --ask-sudo-pass
