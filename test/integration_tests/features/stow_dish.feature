@stow_dish
Feature: Stow SKA dish
  For dish qualification I want to test dish stowing operation
  under different dish mode cases

  Background: Control system is started
  	Given TangoDB is up and running
  	And LMC is up and running

	Scenario Outline: Stow dish in STANDBY-LP
    Given I am an "<LMC>" client (TM or operator)
		And Dish is in STANDBY-LP mode
    When I request dish LMC to stow
    Then I should be acknowledged that stowing was initiated by LMC
		And I should be notified within 30 s that dish is in STOW mode

	Examples: Device name
		|         LMC              | 
		| mid_dish_0001/elt/master |
		| dish_master              | 
