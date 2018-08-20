@set_spf_operational
Feature: Set SPF to OPERATE mode
  For dish qualification I want to test setting SPF in operation
  under different cases

  Background: Control system is started
  	Given "<TangoDB>" is up and running
  	And "<LMC>" is up and running
		And "<SPF>" is up and running

	Scenario Outline: Set SPF to OPERATE mode from STANDBY-LP mode
    Given I am an "<LMC>" client (TM or operator)
		And "<SPF>" is in STANDBY-LP mode
    When I request LMC to set SPF to OPERATE mode  
    Then I should be acknowledged that command was initiated by LMC
		And I should be notified within 30 s that SPF is in OPERATE mode and that command completed
		
	Examples: Device name
		|         LMCDevice              |            SPFDevice                |
		| mid_dish_0001/elt/master       |       mid_dish_0001/spf/controller  |
		
