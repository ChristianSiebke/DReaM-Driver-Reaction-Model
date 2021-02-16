from event_loader import Event


def test_event_generation_triggering():
    e = Event()
    e.triggering.append(1)
    e.triggering.append(2)
    assert(e.eval_triggering_and_affected() ==
           [
        {'AgentId': 1, 'IsTriggering': True, 'IsAffected': False},
        {'AgentId': 2, 'IsTriggering': True, 'IsAffected': False}
    ])


def test_event_generation_affected():
    e = Event()
    e.affected.append(1)
    assert(e.eval_triggering_and_affected() ==
           [
        {'AgentId': 1, 'IsTriggering': False, 'IsAffected': True}
    ])


def test_event_generation_without_parameters():
    e = Event()
    e.meta = {'Meta': 'SomeInfo'}
    e.triggering.append(1)
    e.triggering.append(2)

    assert(e.get() == [{'Meta': 'SomeInfo', 'AgentId': 1,
                        'IsTriggering': True, 'IsAffected': False, 'Key': '', 'Value': ''},
                       {'Meta': 'SomeInfo', 'AgentId': 2,
                        'IsTriggering': True, 'IsAffected': False, 'Key': '', 'Value': ''}])


def test_event_generation_with_parameters():
    e = Event()
    e.meta = {'Meta': 'SomeInfo'}
    e.triggering.append(1)
    e.triggering.append(2)
    e.parameters.update({'key': 'value'})

    assert(e.get() == [{'Meta': 'SomeInfo', 'AgentId': 1,
                        'IsTriggering': True, 'IsAffected': False, 'Key': 'key', 'Value': 'value'},
                       {'Meta': 'SomeInfo', 'AgentId': 2,
                        'IsTriggering': True, 'IsAffected': False, 'Key': 'key', 'Value': 'value'}])


def test_event_generation_with_two_parameters():
    e = Event()
    e.meta = {'Meta': 'SomeInfo'}
    e.triggering.append(1)
    e.triggering.append(2)
    e.parameters.update({'key1': 'value1'})
    e.parameters.update({'key2': 'value2'})

    assert(e.get() == [{'Meta': 'SomeInfo', 'AgentId': 1,
                        'IsTriggering': True, 'IsAffected': False, 'Key': 'key1', 'Value': 'value1'},
                       {'Meta': 'SomeInfo', 'AgentId': 1,
                        'IsTriggering': True, 'IsAffected': False, 'Key': 'key2', 'Value': 'value2'},
                       {'Meta': 'SomeInfo', 'AgentId': 2,
                        'IsTriggering': True, 'IsAffected': False, 'Key': 'key1', 'Value': 'value1'},
                       {'Meta': 'SomeInfo', 'AgentId': 2,
                        'IsTriggering': True, 'IsAffected': False, 'Key': 'key2', 'Value': 'value2'}])
